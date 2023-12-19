// internal
#include <iostream>
#include "render_system.hpp"
#include "SDL.h"

#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include <random>
#include <sstream>

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	Motion &motion = registry.motions.get(entity);

    bool isUI = registry.userInterfaces.has(entity);

	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;

    // Only translate things that are not UI
    if(!isUI && registry.motions.has(camera))
    {
        Motion& cameraMotion = registry.motions.get(camera);
        transform.translate(-cameraMotion.position);
    }

    transform.translate(motion.position);
    transform.rotate(motion.angle);
    transform.scale(motion.scale);

	assert(registry.renderRequests.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)Effects::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == Effects::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		GLuint time_uloc = glGetUniformLocation(program, "u_Time");
		glUniform1f(time_uloc, (float)(glfwGetTime()/10.0f));
	

		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}	else if (render_request.used_effect == Effects::COLOURED)
    {
        GLint in_position_loc = glGetAttribLocation(program, "in_position");
        GLint in_color_loc = glGetAttribLocation(program, "in_color");
        gl_has_errors();

        glEnableVertexAttribArray(in_position_loc);
        glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                              sizeof(ColoredVertex), (void *)0);
        gl_has_errors();

        glEnableVertexAttribArray(in_color_loc);
        glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
                              sizeof(ColoredVertex), (void *)sizeof(vec3));
        gl_has_errors();

		// GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		// GLuint time_uloc = glGetUniformLocation(program, "u_Time");
		// glUniform1f(time_uloc, (float)(glfwGetTime()/10.0f));
	

		// gl_has_errors();
		// assert(in_texcoord_loc >= 0);

		// glEnableVertexAttribArray(in_position_loc);
		// glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
		// 					  sizeof(TexturedVertex), (void *)0);
		// gl_has_errors();

		// glEnableVertexAttribArray(in_texcoord_loc);
		// glVertexAttribPointer(
		// 	in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
		// 	(void *)sizeof(
		// 		vec3)); // note the stride to skip the preceeding vertex position

		// // Enabling and binding texture to slot 0
		// glActiveTexture(GL_TEXTURE0);
		// gl_has_errors();

		// assert(registry.renderRequests.has(entity));
		// GLuint texture_id =
		// 	texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		// glBindTexture(GL_TEXTURE_2D, texture_id);
		// gl_has_errors();

    }
	else if (render_request.used_effect == Effects::PARTICLES)
	{
		GLuint time_uloc = glGetUniformLocation(program, "u_Time");
		glUniform1f(time_uloc, (float)(glfwGetTime()));

		std::vector<TexturedVertex> textured_vertices(4); // Based from render_init.cpp
		textured_vertices[0].position = { -1.f/2, +1.f/2, 0.f }; // top left 0
		textured_vertices[1].position = { -1.f/2, -1.f/2, 0.f }; // bottom left 3
		textured_vertices[2].position = { +1.f/2, +1.f/2, 0.f }; // top right 1 
		textured_vertices[3].position = { +1.f/2, -1.f/2, 0.f }; // bottom right 2

		textured_vertices[0].texcoord = { 0.f, 1.f };
		textured_vertices[1].texcoord = { 0.f, 0.f };
		textured_vertices[2].texcoord = { 1.f, 1.f };
		textured_vertices[3].texcoord = { 1.f, 0.f };

		GLuint vertexBuffer;
		glGenBuffers(1, &vertexBuffer); // generate buffer

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // bind buffer
		glBufferData(GL_ARRAY_BUFFER,
		sizeof(textured_vertices[0]) * textured_vertices.size(), textured_vertices.data(), GL_STATIC_DRAW); // put data into buffer


		gl_has_errors();

		glEnableVertexAttribArray(0); // vertex attribute for positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0); // pointer to the attribute in vertex shader layout(location = *INSERT_INDEX*)
		gl_has_errors();

		glEnableVertexAttribArray(1); // vertex attribute for texcoords
		glVertexAttribPointer(
			1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		glm::vec2 shiftedPositions[] = {
			glm::vec2(0.f, 0.f),   // original position
			glm::vec2(1.0f, 0.f),    // far right
			glm::vec2(0.f, 1.0f),  // top
			glm::vec2(-1.0f, -1.0f)    // bottom left corner
			// CAN ADD MORE POSITIONS
		};

		GLuint shiftedBuffer;
		glGenBuffers(1, &shiftedBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, shiftedBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(shiftedPositions), shiftedPositions, GL_STATIC_DRAW);

		
		glEnableVertexAttribArray(2); // vertex attribute for shifted_positions
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); 
		glVertexAttribDivisor(2, 1);

		gl_has_errors();

		// glm::vec2 shiftedTexCoords[] = {
		// 	glm::vec2(0.0f, 1.0f),  
		// 	glm::vec2(1.0f, 1.0f),  
		// 	glm::vec2(0.0f, 0.0f),  
		// 	glm::vec2(0.0f, 0.0f)   
		// };

		// glEnableVertexAttribArray(3);
		// glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		// glVertexAttribDivisor(3, 1);

		
		std::vector<float> timeChange(4);
		std::default_random_engine generator;
		

		std::uniform_real_distribution<float> rand_time(0.0f, 1.0f);

		// random time
		for (size_t i = 0; i < 4; ++i) {
			timeChange[i] = (rand_time(generator) * 7.5f); // max time
		};

		std::vector<float> scaleVector(4);
		std::uniform_real_distribution<float> rand_scale(0.25f, 1.0f);


		// Generate random scales
		for (size_t i = 0; i < 4; ++i) {
			scaleVector[i] = (rand_scale(generator) * 1.25f); // max scale change
		};
	

		GLuint timeBuffer;
		glGenBuffers(1, &timeBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, timeBuffer);
		glBufferData(GL_ARRAY_BUFFER, timeChange.size() * sizeof(float), timeChange.data(), GL_STATIC_DRAW);
		
		
		glEnableVertexAttribArray(4); // vertex attribute for time changing
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
		glVertexAttribDivisor(4, 1);

		
		GLuint scaleBuffer;
		glGenBuffers(1, &scaleBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, scaleBuffer);
		glBufferData(GL_ARRAY_BUFFER, scaleVector.size() * sizeof(float), scaleVector.data(), GL_STATIC_DRAW);
		
		
		glEnableVertexAttribArray(5); // vertex attribute for scale changing
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
		glVertexAttribDivisor(5, 1); 
	

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 4); // DRAW STARTING AT INDEX 0, 4 VERTEXES, 4 INSTANCES(OBJECTS)

		gl_has_errors();
	}
	else
	{	
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

    for(const std::string& uniformName : iUniforms){
        GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());

        if(render_request.iUniforms.find(uniformName) == render_request.iUniforms.end()){
            glUniform1i(uniformLocation, 0);
        }
    }

    for(const std::string& uniformName : fUniforms){
        GLint uniformLocation = glGetUniformLocation(program, uniformName.c_str());

        if(render_request.fUniforms.find(uniformName) == render_request.fUniforms.end()){
            glUniform1f(uniformLocation, 0.0f);
        }
    }

    GLint spriteSheetDimensions = glGetUniformLocation(program, "u_SpriteSheetSize");
    GLint spriteSheetSelection = glGetUniformLocation(program, "u_SpriteSheetSelection");

    if(registry.spriteSheets.has(entity)){
        SpriteSheet& spriteSheet = registry.spriteSheets.get(entity);

        glUniform2f(spriteSheetDimensions, spriteSheet.dimensions.x, spriteSheet.dimensions.y);
        glUniform2f(spriteSheetSelection, spriteSheet.current.x, spriteSheet.current.y);
    }else{
        glUniform2f(spriteSheetDimensions, 1.0f, 1.0f);
        glUniform2f(spriteSheetSelection, 0.0f, 0.0f);
    }

    for(const auto& uniform : render_request.iUniforms){
        GLint uniformLocation = glGetUniformLocation(program, uniform.first.c_str());

        glUniform1i(uniformLocation, uniform.second);
        gl_has_errors("RenderSystem::drawTexturedMesh - failed to set integer uniform. Is it defined as an integer?");

        if(std::find(iUniforms.begin(), iUniforms.end(), uniform.first) == iUniforms.end()){
            iUniforms.push_back(uniform.first);
            std::cout << "Registered integer uniform " << uniform.first << std::endl;
        }

    }

    for(const auto& uniform : render_request.fUniforms){
        GLint uniformLocation = glGetUniformLocation(program, uniform.first.c_str());

        glUniform1f(uniformLocation, uniform.second);
        gl_has_errors("RenderSystem::drawTexturedMesh - failed to set float uniform. Is it defined as a float?");

        if(std::find(fUniforms.begin(), fUniforms.end(), uniform.first) == fUniforms.end()){
            fUniforms.push_back(uniform.first);
            std::cout << "Registered float uniform " << uniform.first << std::endl;
        }
    }

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// water
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the water texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)Effects::POST_PROCESS]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(0.f, 0, 0, 0.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint water_program = effects[(GLuint)Effects::POST_PROCESS];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(water_program, "time");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(water_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

   // int w = game_width;
   // int h = game_height;

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors("RenderSystem::draw");
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0, 0, 1, 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors("RenderSystem::draw");
	mat3 projection_2D = createProjectionMatrix();

    Motion& cameraMotion = registry.motions.get(camera);

    // Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		if (!registry.motions.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawTexturedMesh(entity, projection_2D);

        if(registry.texts.has(entity)){
            Text& text = registry.texts.get(entity);
            Motion& motion = registry.motions.get(entity);

            vec2 pos = motion.position + text.offset;

            // If we aren't UI then transform with the camera
            if(!registry.userInterfaces.has(entity)){
                pos -= cameraMotion.position;
            }

            drawText(text.font, text.text, pos.x, pos.y, text.scale, text.color, text.shadow);
        }

	}

    drawDebugText();

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors("RenderSystem::draw");
}

void RenderSystem::drawDebugText() {

    if(!WorldSystem::isMiniGameActive()){
        Motion& cameraMotion = registry.motions.get(worldSystem->getCamera());
        Motion& buddy = registry.motions.get(worldSystem->getBuddy());

        float distanceFromPlayer = 0.0f;
        bool isOffLeftScreen = false;

        if(buddy.position.x < cameraMotion.position.x){
            distanceFromPlayer = glm::abs(cameraMotion.position.x - buddy.position.x);
            isOffLeftScreen = true;
        }else if(buddy.position.x > cameraMotion.position.x + game_width){
            distanceFromPlayer = glm::abs(cameraMotion.position.x + game_width - buddy.position.x);
        }

        if(distanceFromPlayer > 0){

            float meters = distanceFromPlayer / 100.0f;

            std::stringstream stream;
            // Format the float with one decimal place
            stream << std::fixed << std::setprecision(1) << meters;
            // Convert the stringstream to a string
            std::string formattedString = stream.str();

            if(isOffLeftScreen){
                drawText(Fonts::Retro, "<-- " + formattedString + "m", 10, game_height - 150, 0.5f, vec3(1, 1, 1), true);
            }else{
                drawText(Fonts::Retro, formattedString + "m -->", game_width - 120, game_height - 150, 0.5f, vec3(1, 1, 1), true);
            }

        }
    }


    //drawText(Fonts::Retro, "Buddy Age: " + std::to_string((int) (buddyStatusSystem->age / 60.0f) + 1), 50, 100, 0.5f, vec3(1.0f), true);
}

void RenderSystem::drawText(Fonts desiredFont, const std::string& text, float x, float y, float scale, glm::vec3 color, bool shadow) {
    if(shadow){
        drawText(desiredFont, text, x + 2, y + 2, scale, glm::mix(color, vec3(0.0), 0.45f));
        drawText(desiredFont, text, x, y, scale, color);
    }else{
        drawText(desiredFont, text, x, y, scale, color);
    }
}

void RenderSystem::drawText(Fonts desiredFont, const std::string& text, float x, float y, float scale, glm::vec3 color) {

    Font& font = fonts[desiredFont];

    const GLuint program = effects[(int) Effects::TEXT_SHADER];

    mat3 projection = createProjectionMatrix();

    // Setup shader
    glUseProgram(program);
    glUniform3f(glGetUniformLocation(program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    gl_has_errors();

    GLuint projection_loc = glGetUniformLocation(program, "projection");
    glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);

    gl_has_errors();

    // update content of VBO memory
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    gl_has_errors();

    GLint vertexLocation = glGetAttribLocation(program, "vertex");

    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    gl_has_errors();

    const float initialX = x;

    std::string::const_iterator c;
    for(c = text.begin();c != text.end();c++){

        char character = *c;

        if(font.characters.find(character) == font.characters.end()){
            continue;
        }

        Character ch = font.characters[character];

        float xpos = x + (float) ch.Bearing.x * scale;
        float ypos = y - (float) ch.Bearing.y * scale + 20; // Something went wonky, add 20 units to fix it

        float w = (float) ch.Size.x * scale;
        float h = (float) ch.Size.y * scale;

        if(character == '\n'){
            y += h + 5;
            x = initialX;
            continue;
        }

        float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 1.0f },
                { xpos,     ypos,       0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 0.0f },

                { xpos,     ypos + h,   0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 0.0f },
                { xpos + w, ypos + h,   1.0f, 1.0f }
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        gl_has_errors();

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        gl_has_errors();

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        gl_has_errors();

        x += (float) (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors("RenderSystem::createProjectionMatrix");
	float right = (float) game_width;
	float bottom = (float) game_height;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}
