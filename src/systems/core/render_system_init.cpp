// internal
#include "render_system.hpp"

#include <array>
#include <fstream>
#include "stb_image.h"

// This creates circular header inclusion, that is quite bad.
#include "engine/tiny_ecs_registry.hpp"

#include "engine/text_rendering.h"

// stlib
#include <iostream>
#include <sstream>

// World initialization
bool RenderSystem::init(GLFWwindow* window_arg, Entity cameraEntity)
{
	this->window = window_arg;
    initializeTexturePaths();

    this->camera = cameraEntity;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	const int is_fine = gl3w_init();
	assert(is_fine == 0);

	// Create a frame buffer
	frame_buffer = 0;
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// For some high DPI displays (ex. Retina Display on Macbooks)
	// https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value
	int frame_buffer_width_px, frame_buffer_height_px;
	glfwGetFramebufferSize(window, &frame_buffer_width_px, &frame_buffer_height_px);  // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	if (frame_buffer_width_px != window_width_px)
	{
		printf("WARNING: retina display! https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value\n");
		printf("glfwGetFramebufferSize = %d,%d\n", frame_buffer_width_px, frame_buffer_height_px);
		printf("window width_height = %d,%d\n", window_width_px, window_height_px);
	}

	// Hint: Ask your TA for how to setup pretty OpenGL error callbacks. 
	// This can not be done in mac os, so do not enable
	// it unless you are on Linux or Windows. You will need to change the window creation
	// code to use OpenGL 4.3 (not suported on mac) and add additional .h and .cpp
	// glDebugMessageCallback((GLDEBUGPROC)errorCallback, nullptr);

	// We are not really using VAO's but without at least one bound we will crash in
	// some systems.
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	gl_has_errors();

	initScreenTexture();
    initializeGlTextures();
	initializeGlEffects();
	initializeGlGeometryBuffers();

    Font defaultFont;

    if(loadFont(font_path("default.ttf"), defaultFont) != 0){
        std::cerr << "Failed to load font!" << std::endl;
    }
	Font retroFont;
	if (loadFont(font_path("retro_gaming.ttf"), retroFont) != 0) {
		std::cerr << "Failed to load retro font!" << std::endl;
	}

    fonts[Fonts::Default] = defaultFont;
	fonts[Fonts::Retro] = retroFont;

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    return true;
}

void RenderSystem::initializeGlTextures()
{
    glGenTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());

    for(auto entry : texture_paths)
    {
		const std::string& path = entry.second;
		ivec2& dimensions = texture_dimensions[(int) entry.first];

		stbi_uc* data;
		data = stbi_load(path.c_str(), &dimensions.x, &dimensions.y, NULL, 4);

		if (data == NULL)
		{
			const std::string message = "Could not load the file " + path + ".";
			fprintf(stderr, "%s", message.c_str());
			assert(false);
		}
		glBindTexture(GL_TEXTURE_2D, texture_gl_handles[(int) entry.first]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		gl_has_errors();
		stbi_image_free(data);
    }
	gl_has_errors();
}

void RenderSystem::initializeGlEffects()
{
	for(uint i = 0; i < effect_paths.size(); i++)
	{
		const std::string vertex_shader_name = effect_paths[i] + ".vs.glsl";
		const std::string fragment_shader_name = effect_paths[i] + ".fs.glsl";

		bool is_valid = loadEffectFromFile(vertex_shader_name, fragment_shader_name, effects[i]);
		assert(is_valid && (GLuint)effects[i] != 0);
	}
}

// One could merge the following two functions as a template function...
template <class T>
void RenderSystem::bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(uint)gid]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	gl_has_errors();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[(uint)gid]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
	gl_has_errors();
}

void RenderSystem::initializeGlMeshes()
{
	for (uint i = 0; i < mesh_paths.size(); i++)
	{
		// Initialize meshes
		GEOMETRY_BUFFER_ID geom_index = mesh_paths[i].first;
		std::string name = mesh_paths[i].second;
		Mesh::loadFromOBJFile(name, 
			meshes[(int)geom_index].vertices,
			meshes[(int)geom_index].vertex_indices,
			meshes[(int)geom_index].original_size);

		bindVBOandIBO(geom_index,
			meshes[(int)geom_index].vertices, 
			meshes[(int)geom_index].vertex_indices);
	}
}

void RenderSystem::initializeGlGeometryBuffers()
{
	// Vertex Buffer creation.
	glGenBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	// Index Buffer creation.
	glGenBuffers((GLsizei)index_buffers.size(), index_buffers.data());

	// Index and Vertex buffer data initialization.
	initializeGlMeshes();

	//////////////////////////
	// Initialize sprite
	// The position corresponds to the center of the texture.
	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f/2, +1.f/2, 0.f };
	textured_vertices[1].position = { +1.f/2, +1.f/2, 0.f };
	textured_vertices[2].position = { +1.f/2, -1.f/2, 0.f };
	textured_vertices[3].position = { -1.f/2, -1.f/2, 0.f };
	textured_vertices[0].texcoord = { 0.f, 1.f };
	textured_vertices[1].texcoord = { 1.f, 1.f };
	textured_vertices[2].texcoord = { 1.f, 0.f };
	textured_vertices[3].texcoord = { 0.f, 0.f };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SPRITE, textured_vertices, textured_indices);

	////////////////////////
	// Initialize pebble
	std::vector<ColoredVertex> pebble_vertices;
	std::vector<uint16_t> pebble_indices;
	constexpr float z = -0.1f;
	constexpr int NUM_TRIANGLES = 62;

	for (int i = 0; i < NUM_TRIANGLES; i++) {
		const float t = float(i) * M_PI * 2.f / float(NUM_TRIANGLES - 1);
		pebble_vertices.push_back({});
		pebble_vertices.back().position = { 0.5 * cos(t), 0.5 * sin(t), z };
		pebble_vertices.back().color = { 0.8, 0.8, 0.8 };
	}
	pebble_vertices.push_back({});
	pebble_vertices.back().position = { 0, 0, 0 };
	pebble_vertices.back().color = { 0.8, 0.8, 0.8 };
	for (int i = 0; i < NUM_TRIANGLES; i++) {
		pebble_indices.push_back((uint16_t)i);
		pebble_indices.push_back((uint16_t)((i + 1) % NUM_TRIANGLES));
		pebble_indices.push_back((uint16_t)NUM_TRIANGLES);
	}
	int geom_index = (int)GEOMETRY_BUFFER_ID::PEBBLE;
	meshes[geom_index].vertices = pebble_vertices;
	meshes[geom_index].vertex_indices = pebble_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::PEBBLE, meshes[geom_index].vertices, meshes[geom_index].vertex_indices);

	//////////////////////////////////
	// Initialize debug line
	std::vector<ColoredVertex> line_vertices;
	std::vector<uint16_t> line_indices;

	constexpr float depth = 0.5f;
	constexpr vec3 red = { 0.8,0.1,0.1 };

	// Corner points
	line_vertices = {
		{{-0.5,-0.5, depth}, red},
		{{-0.5, 0.5, depth}, red},
		{{ 0.5, 0.5, depth}, red},
		{{ 0.5,-0.5, depth}, red},
	};

	// Two triangles
	line_indices = {0, 1, 3, 1, 2, 3};
	
	geom_index = (int)GEOMETRY_BUFFER_ID::DEBUG_LINE;
	meshes[geom_index].vertices = line_vertices;
	meshes[geom_index].vertex_indices = line_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::DEBUG_LINE, line_vertices, line_indices);

	///////////////////////////////////////////////////////
	// Initialize screen triangle (yes, triangle, not quad; its more efficient).
	std::vector<vec3> screen_vertices(3);
	screen_vertices[0] = { -1, -6, 0.f };
	screen_vertices[1] = { 6, -1, 0.f };
	screen_vertices[2] = { -1, 6, 0.f };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> screen_indices = { 0, 1, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE, screen_vertices, screen_indices);
}

RenderSystem::~RenderSystem()
{
	// Don't need to free gl resources since they last for as long as the program,
	// but it's polite to clean after yourself.
	glDeleteBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	glDeleteBuffers((GLsizei)index_buffers.size(), index_buffers.data());
	glDeleteTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());
	glDeleteTextures(1, &off_screen_render_buffer_color);
	glDeleteRenderbuffers(1, &off_screen_render_buffer_depth);
	gl_has_errors();

	for(uint i = 0; i < effect_count; i++) {
		glDeleteProgram(effects[i]);
	}
	// delete allocated resources
	glDeleteFramebuffers(1, &frame_buffer);
	gl_has_errors();

	// remove all entities created by the render system
	while (registry.renderRequests.entities.size() > 0)
	    registry.remove_all_components_of(registry.renderRequests.entities.back());

    // dispose fonts
    for(auto& pair : fonts){
        for(auto& character : pair.second.characters){
            glDeleteTextures(1, &character.second.TextureID);
        }
    }

}

// Initialize the screen texture from a standard sprite
bool RenderSystem::initScreenTexture()
{
	int framebuffer_width, framebuffer_height;
	glfwGetFramebufferSize(const_cast<GLFWwindow*>(window), &framebuffer_width, &framebuffer_height);  // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	glGenTextures(1, &off_screen_render_buffer_color);
	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebuffer_width, framebuffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl_has_errors();

	glGenRenderbuffers(1, &off_screen_render_buffer_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, off_screen_render_buffer_depth);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, off_screen_render_buffer_color, 0);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebuffer_width, framebuffer_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, off_screen_render_buffer_depth);
	gl_has_errors();

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	return true;
}

void RenderSystem::initializeTexturePaths() {
    // character textures
    texture_paths[Textures::TEST_GUY] = textures_path("guy.png");

	// mail textures
	texture_paths[Textures::MAIL_ICON] = textures_path("mail.png");
    texture_paths[Textures::MAIL_NOTIF_ICON] = textures_path("mailNotif.png");
    texture_paths[Textures::POSTCARD] = textures_path("postcard.png");

	// intro cut scene textures
	texture_paths[Textures::TUTSKY1] = textures_path("intro_cut_scene/tutSky1.png");
	texture_paths[Textures::TUTSKY2] = textures_path("intro_cut_scene/tutSky2.png");
	texture_paths[Textures::TUTBACKGROUND] = textures_path("intro_cut_scene/tutBackground.png");
	texture_paths[Textures::EGG_IDLE] = textures_path("intro_cut_scene/eggIdleSpriteSheet.png");
	texture_paths[Textures::EGG_HATCH] = textures_path("intro_cut_scene/eggHatchSpriteSheet.png");

	// room textures
    texture_paths[Textures::LIVING_ROOM] = textures_path("livingRoom.png");
    texture_paths[Textures::KITCHEN_ROOM] = textures_path("kitchenRoom.png");
    texture_paths[Textures::BACKYARD] = textures_path("backyard.png");
	texture_paths[Textures::BATH_ROOM] = textures_path("bathroom.png");

	// kitchen-specific textures
    texture_paths[Textures::FRIDGE] = textures_path("fridge.png");
    texture_paths[Textures::FRIDGE_OPEN] = textures_path("fridge_open.png");

	// kitchen game textures
	texture_paths[Textures::PAN] = textures_path("kitchen_game/pan.png");
	texture_paths[Textures::PLATE] = textures_path("kitchen_game/plate2.png");
	texture_paths[Textures::PANCAKE] = textures_path("kitchen_game/pancake.png");
	texture_paths[Textures::DAWNSKY1] = textures_path("dawnSky1.png");
	texture_paths[Textures::DAWNSKY2] = textures_path("dawnSky2.png");
	texture_paths[Textures::DAWNSKY3] = textures_path("dawnSky3.png");
	texture_paths[Textures::DAWNSKY4] = textures_path("dawnSky4.png");

	// backyard textures
    texture_paths[Textures::SEED_PLOT] = textures_path("seedPlot.png");
	texture_paths[Textures::TREE_DOOR] = textures_path("treeDoor.png");
    texture_paths[Textures::CARROT] = textures_path("carrot.png");
    texture_paths[Textures::MELON] = textures_path("melon.png");
    texture_paths[Textures::BANANA] = textures_path("banana.png");
    texture_paths[Textures::PUMPKIN] = textures_path("pumpkin.png");
    texture_paths[Textures::FERTILIZER] = textures_path("fertilizer.png");
    texture_paths[Textures::FERTILIZER_SHADOW] = textures_path("fertilizerShadow.png");


	// backyard game textures
    texture_paths[Textures::BACKYARD_GAME_BACKGROUND] = textures_path("backyard_game/treeGameBackground.png");
    texture_paths[Textures::GUN] = textures_path("backyard_game/waterGun.png");
    texture_paths[Textures::BULLET] = textures_path("backyard_game/waterBullet.png");
    texture_paths[Textures::SQUIRREL_1] = textures_path("backyard_game/squirrel1.png");
    texture_paths[Textures::SQUIRREL_2] = textures_path("backyard_game/squirrel2.png");
    texture_paths[Textures::SQUIRREL_SPRITESHEET] = textures_path("backyard_game/squirrelSpriteSheet.png");
    texture_paths[Textures::SQUIRREL_HIT] = textures_path("backyard_game/squirrelHit.png");

	// bedroom textures
	texture_paths[Textures::SHEEP_SPRITESHEET] = textures_path("sheepSpriteSheet.png");
	texture_paths[Textures::BEDROOM] = textures_path("bedroom.png");
	texture_paths[Textures::DREAM_BUBBLE] = textures_path("dreamBubble.png");
	texture_paths[Textures::SHEEP1] = textures_path("sheep1.png");
	texture_paths[Textures::DREAM_GAME_GROUND] = textures_path("dreamGameGround.png");
	texture_paths[Textures::GUY_RUN_SPRITESHEET] = textures_path("guyRunSpritesheet.png");
	texture_paths[Textures::GUY_SLEEP_SPRITESHEET] = textures_path("guySleepSpritesheet.png");
	texture_paths[Textures::GUY_JUMP] = textures_path("guyJump.png");
	texture_paths[Textures::GUY_DEATH] = textures_path("guyDeath.png");
	texture_paths[Textures::RABBIT1] = textures_path("rabbit1.png");
	texture_paths[Textures::RABBIT2] = textures_path("rabbit2.png");
	texture_paths[Textures::RABBIT_SPRITESHEET] = textures_path("rabbitSpriteSheet.png");
	texture_paths[Textures::TWISKY1] = textures_path("twiSky1.png");
	texture_paths[Textures::TWISKY2] = textures_path("twiSky2.png");
	texture_paths[Textures::TWISKY3] = textures_path("twiSky3.png");
	texture_paths[Textures::TWISKY4] = textures_path("twiSky4.png");
	texture_paths[Textures::TWISKY5] = textures_path("twiSky5.png");
	texture_paths[Textures::LAMP] = textures_path("bedLamp.png");

	// bathroom textures
	texture_paths[Textures::POOP] = textures_path("poop.png");
	texture_paths[Textures::TOILET] = textures_path("toilet.png");

	// bathroom mini game textures
	texture_paths[Textures::BATHROOM_GAME_BACKGROUND] = textures_path("bathroom_game/pooGameSky1.png");
	texture_paths[Textures::BATHROMM_GAME_CLOUD] = textures_path("bathroom_game/pooGameSky2.png");
	texture_paths[Textures::GIFTBOX_CLOSE] = textures_path("bathroom_game/giftboxClosed.png");
	texture_paths[Textures::GIFTBOX_OPEN] = textures_path("bathroom_game/giftboxOpen.png");
	texture_paths[Textures::SOAP] = textures_path("soap.png");
    
    // other background textures
	texture_paths[Textures::BLUESKY1] = textures_path("blueSky1.png");
	texture_paths[Textures::BLUESKY2] = textures_path("blueSky2.png");
	texture_paths[Textures::BLUESKY3] = textures_path("blueSky3.png");
	texture_paths[Textures::BLUESKY4] = textures_path("blueSky4.png");
	texture_paths[Textures::MOUNTAIN1] = textures_path("mountain1.png");
	texture_paths[Textures::MOUNTAIN2] = textures_path("mountain2.png");
	texture_paths[Textures::MOUNTAIN3] = textures_path("mountain3.png");
	texture_paths[Textures::NIGHTSKY1] = textures_path("nightSky1.png");
	texture_paths[Textures::NIGHTSKY2] = textures_path("nightSky2.png");
	texture_paths[Textures::NIGHTSKY3] = textures_path("nightSky3.png");
	
	// navigation textures
    texture_paths[Textures::CLOSE] = textures_path("close.png");
	texture_paths[Textures::ENTERTAINMENT] = textures_path("status_textures/entertainment.png");
	texture_paths[Textures::HYGIENE] = textures_path("status_textures/hygiene.png");
	texture_paths[Textures::REST] = textures_path("status_textures/rest.png");
	texture_paths[Textures::SATIATION] = textures_path("status_textures/satiation.png");
    texture_paths[Textures::STAR] = textures_path("star.png");
	texture_paths[Textures::BASKET] = textures_path("basket.png");

    //Tutorial
    texture_paths[Textures::LEFT_KEY] = textures_path("tutorial/left_key.png");
    texture_paths[Textures::RIGHT_KEY] = textures_path("tutorial/right_key.png");

    //stuff
    texture_paths[Textures::WHISTLE] = textures_path("whistle.png");

    // Buddy textures
    texture_paths[Textures::BUDDY_BABY_DEATH] = textures_path("baby/death.png");
    texture_paths[Textures::BUDDY_BABY_IDLE] = textures_path("baby/idle.png");
    texture_paths[Textures::BUDDY_BABY_JUMP] = textures_path("baby/jump.png");
    texture_paths[Textures::BUDDY_BABY_RUN] = textures_path("baby/run.png");
    texture_paths[Textures::BUDDY_BABY_SLEEP] = textures_path("baby/sleep.png");

    texture_paths[Textures::BUDDY_SMART_DEATH] = textures_path("teen/teenDeathSmart.png");
    texture_paths[Textures::BUDDY_SMART_IDLE] = textures_path("teen/teenIdleSmartSpriteSheet.png");
    texture_paths[Textures::BUDDY_SMART_JUMP] = textures_path("teen/teenJumpSmart.png");
    texture_paths[Textures::BUDDY_SMART_RUN] = textures_path("teen/teenRunSmartSpriteSheet.png");
    texture_paths[Textures::BUDDY_SMART_SLEEP] = textures_path("teen/teenSleepSmartSpriteSheet.png");

    texture_paths[Textures::BUDDY_CUTE_DEATH] = textures_path("teen/teenDeathCute.png");
    texture_paths[Textures::BUDDY_CUTE_IDLE] = textures_path("teen/teenIdleCuteSpriteSheet.png");
    texture_paths[Textures::BUDDY_CUTE_JUMP] = textures_path("teen/teenJumpCute.png");
    texture_paths[Textures::BUDDY_CUTE_RUN] = textures_path("teen/teenRunCuteSpriteSheet.png");
    texture_paths[Textures::BUDDY_CUTE_SLEEP] = textures_path("teen/teenSleepCuteSpriteSheet.png");

    texture_paths[Textures::BUDDY_JOCK_DEATH] = textures_path("teen/teenDeathJock.png");
    texture_paths[Textures::BUDDY_JOCK_IDLE] = textures_path("teen/teenIdleJockSpriteSheet.png");
    texture_paths[Textures::BUDDY_JOCK_JUMP] = textures_path("teen/teenJumpJock.png");
    texture_paths[Textures::BUDDY_JOCK_RUN] = textures_path("teen/teenRunJockSpriteSheet.png");
    texture_paths[Textures::BUDDY_JOCK_SLEEP] = textures_path("teen/teenSleepJockSpriteSheet.png");

    texture_paths[Textures::BUDDY_COOL_DEATH] = textures_path("teen/teenDeathCool.png");
    texture_paths[Textures::BUDDY_COOL_IDLE] = textures_path("teen/teenIdleCoolSpriteSheet.png");
    texture_paths[Textures::BUDDY_COOL_JUMP] = textures_path("teen/teenJumpCool.png");
    texture_paths[Textures::BUDDY_COOL_RUN] = textures_path("teen/teenRunCoolSpriteSheet.png");
    texture_paths[Textures::BUDDY_COOL_SLEEP] = textures_path("teen/teenSleepCoolSpriteSheet.png");

    texture_paths[Textures::BUDDY_GOLD_DEATH] = textures_path("teen/teenDeathGold.png");
    texture_paths[Textures::BUDDY_GOLD_IDLE] = textures_path("teen/teenIdleGoldSpriteSheet.png");
    texture_paths[Textures::BUDDY_GOLD_JUMP] = textures_path("teen/teenJumpGold.png");
    texture_paths[Textures::BUDDY_GOLD_RUN] = textures_path("teen/teenRunGoldSpriteSheet.png");
    texture_paths[Textures::BUDDY_GOLD_SLEEP] = textures_path("teen/teenSleepGoldSpriteSheet.png");

    texture_paths[Textures::BUDDY_HOBO_DEATH] = textures_path("teen/teenDeathHobo.png");
    texture_paths[Textures::BUDDY_HOBO_IDLE] = textures_path("teen/teenIdleHoboSpriteSheet.png");
    texture_paths[Textures::BUDDY_HOBO_JUMP] = textures_path("teen/teenJumpHobo.png");
    texture_paths[Textures::BUDDY_HOBO_RUN] = textures_path("teen/teenRunHoboSpriteSheet.png");
    texture_paths[Textures::BUDDY_HOBO_SLEEP] = textures_path("teen/teenSleepHoboSpriteSheet.png");

    texture_paths[Textures::BUDDY_ADULT_SMART_DEATH] = textures_path("adult/adultDeathSmart.png");
    texture_paths[Textures::BUDDY_ADULT_SMART_IDLE] = textures_path("adult/adultIdleSmartSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_SMART_JUMP] = textures_path("adult/adultJumpSmart.png");
    texture_paths[Textures::BUDDY_ADULT_SMART_RUN] = textures_path("adult/adultRunSmartSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_SMART_SLEEP] = textures_path("adult/adultSleepSmartSpriteSheet.png");

    texture_paths[Textures::BUDDY_ADULT_CUTE_DEATH] = textures_path("adult/adultDeathCute.png");
    texture_paths[Textures::BUDDY_ADULT_CUTE_IDLE] = textures_path("adult/adultIdleCuteSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_CUTE_JUMP] = textures_path("adult/adultJumpCute.png");
    texture_paths[Textures::BUDDY_ADULT_CUTE_RUN] = textures_path("adult/adultRunCuteSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_CUTE_SLEEP] = textures_path("adult/adultSleepCuteSpriteSheet.png");

    texture_paths[Textures::BUDDY_ADULT_JOCK_DEATH] = textures_path("adult/adultDeathJock.png");
    texture_paths[Textures::BUDDY_ADULT_JOCK_IDLE] = textures_path("adult/adultIdleJockSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_JOCK_JUMP] = textures_path("adult/adultJumpJock.png");
    texture_paths[Textures::BUDDY_ADULT_JOCK_RUN] = textures_path("adult/adultRunJockSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_JOCK_SLEEP] = textures_path("adult/adultSleepJockSpriteSheet.png");

    texture_paths[Textures::BUDDY_ADULT_COOL_DEATH] = textures_path("adult/adultDeathCool.png");
    texture_paths[Textures::BUDDY_ADULT_COOL_IDLE] = textures_path("adult/adultIdleCoolSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_COOL_JUMP] = textures_path("adult/adultJumpCool.png");
    texture_paths[Textures::BUDDY_ADULT_COOL_RUN] = textures_path("adult/adultRunCoolSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_COOL_SLEEP] = textures_path("adult/adultSleepCoolSpriteSheet.png");

    texture_paths[Textures::BUDDY_ADULT_GOLD_DEATH] = textures_path("adult/adultDeathGold.png");
    texture_paths[Textures::BUDDY_ADULT_GOLD_IDLE] = textures_path("adult/adultIdleGoldSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_GOLD_JUMP] = textures_path("adult/adultJumpGold.png");
    texture_paths[Textures::BUDDY_ADULT_GOLD_RUN] = textures_path("adult/adultRunGoldSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_GOLD_SLEEP] = textures_path("adult/adultSleepGoldSpriteSheet.png");

    texture_paths[Textures::BUDDY_ADULT_HOBO_DEATH] = textures_path("adult/adultDeathHobo.png");
    texture_paths[Textures::BUDDY_ADULT_HOBO_IDLE] = textures_path("adult/adultIdleHoboSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_HOBO_JUMP] = textures_path("adult/adultJumpHobo.png");
    texture_paths[Textures::BUDDY_ADULT_HOBO_RUN] = textures_path("adult/adultRunHoboSpriteSheet.png");
    texture_paths[Textures::BUDDY_ADULT_HOBO_SLEEP] = textures_path("adult/adultSleepHoboSpriteSheet.png");

	// other textures
	texture_paths[Textures::CHATBOX] = textures_path("chatBox.png");
	texture_paths[Textures::CHATBOX_SPRITESHEET] = textures_path("chatBoxSpriteSheet.png");
}

bool gl_compile_shader(GLuint shader)
{
	glCompileShader(shader);
	gl_has_errors();
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint log_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
		std::vector<char> log(log_len);
		glGetShaderInfoLog(shader, log_len, &log_len, log.data());
		glDeleteShader(shader);

		gl_has_errors();

		fprintf(stderr, "GLSL: %s", log.data());
		return false;
	}

	return true;
}

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program)
{
	// Opening files
	std::ifstream vs_is(vs_path);
	std::ifstream fs_is(fs_path);
	if (!vs_is.good() || !fs_is.good())
	{
		fprintf(stderr, "Failed to load shader files %s, %s", vs_path.c_str(), fs_path.c_str());
		assert(false);
		return false;
	}

	// Reading sources
	std::stringstream vs_ss, fs_ss;
	vs_ss << vs_is.rdbuf();
	fs_ss << fs_is.rdbuf();
	std::string vs_str = vs_ss.str();
	std::string fs_str = fs_ss.str();
	const char* vs_src = vs_str.c_str();
	const char* fs_src = fs_str.c_str();
	GLsizei vs_len = (GLsizei)vs_str.size();
	GLsizei fs_len = (GLsizei)fs_str.size();

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs_src, &vs_len);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs_src, &fs_len);
	gl_has_errors();

	// Compiling
	if (!gl_compile_shader(vertex))
	{
		fprintf(stderr, "Vertex compilation failed");
		assert(false);
		return false;
	}
	if (!gl_compile_shader(fragment))
	{
		fprintf(stderr, "Vertex compilation failed");
		assert(false);
		return false;
	}

	// Linking
	out_program = glCreateProgram();
	glAttachShader(out_program, vertex);
	glAttachShader(out_program, fragment);
	glLinkProgram(out_program);
	gl_has_errors();

	{
		GLint is_linked = GL_FALSE;
		glGetProgramiv(out_program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint log_len;
			glGetProgramiv(out_program, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetProgramInfoLog(out_program, log_len, &log_len, log.data());
			gl_has_errors();

			fprintf(stderr, "Link error: %s", log.data());
			assert(false);
			return false;
		}
	}

	// No need to carry this around. Keeping these objects is only useful if we recycle
	// the same shaders over and over, which we don't, so no need and this is simpler.
	glDetachShader(out_program, vertex);
	glDetachShader(out_program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	gl_has_errors();

	return true;
}

