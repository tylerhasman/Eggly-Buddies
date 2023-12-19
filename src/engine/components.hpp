#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include <functional>
#include "stb_image.h"
#include "systems/ai/state_machine.h"

enum class Fonts {
    Default,
	Retro
};

// All data relevant to the shape and motion of entities
struct Motion {
    vec2 lastPhysicsPosition = {0.0f, 0.0f};
	vec2 position = { 0.f, 0.f };
	float angle = 0.f;
	vec2 velocity = { 0.f, 0.f };
	vec2 scale = { 10.f, 10.f };

    bool enableMeshCollision = false;
};

// Player component
struct Player
{
    bool enableGravity = true;
};

struct Text{
    std::string text = "";
    vec2 offset = vec2(0, 0);
    bool centerText = false; // TODO: Implement text centering
    vec3 color = vec3(1.0f);
    Fonts font = Fonts::Default;
    float scale = 1.0f;
    bool shadow = false;
};

// eggly buddy things
struct EgglyStatus 
{
	// status tag? currently only attached to player entity
	// dirtiness, tiredness, hunger all attached to player entity only rn
};

struct Satiation {
	// tag to identify bar
};

struct Hygiene {
	// tag to identify bar
};

struct Rest {
	// tag to identify bar
};

struct Entertainment {
	// tag to identify bar
};

struct StatusBar {

};

struct Camera {

};

struct UserInterface {

};

// MailEvent component
struct MailEvent
{

};

// SleepState component
struct SleepState {

};

struct KitchenEvent
{

};

struct AiController {
    StateMachine stateMachine;
};

struct DreamEnemy {
	float value = 0.f;
};

struct FallingFood {

	bool isStackedOnSomething = false;

	bool isStackableOn = false;

	Entity stackedOn;

    int numberInStack = 1;

    float offsetX = 0.0f;
    float velocityX = 0.0f;
};

struct KitchenAppliance
{

};

struct Food {

};

struct Book{
	bool isSelected = false;
	bool BathroomGameItem = false;

};


struct Clickable
{
    std::function<void(Entity)> onClick = nullptr;
    bool highlightOnHover = true;
};

struct Pickupable
{
    bool pickedUp = false;
};

struct PickupReceiver {
    // Function that is called when a player drops a pickup onto this other entity
    std::function<void(Entity)> onPickupReceived = nullptr;
};

struct Growable
{
	Entity seedPlot;
	Entity particles;
	float growthTime = 0.f;
	float initialHeight = 0.f;
	float finalHeight = 0.f;
	float remainingGrowthTime = 0.f;
	bool grown = false;
};

struct Fertilizer {

};

struct Projectile 
{

};

struct DeathTimer {
	
};



struct BackyardEnemy {
	Entity imageOverlay;
	float health = 2.f;
	float angle;
	Entity currentProjectile;
	vec2 pathPosition;
	vec2 hardCorePathPosition;
	bool aboutToDodge = false;
	bool isDodging = false;
	vec2 dodgePosition;
	vec2 blockPosition;
	Entity ally;
};

struct HitTimer {
	float timer = 0.75;
};

struct UserHelp {

};

struct Boundary
{
	vec2 minBoundary = {-10000, -10000};
	vec2 maxBoundary = {10000, 10000};
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other_entity; // the second object involved in the collision
    vec2 normal;
	Collision(Entity& other_entity) {
        this->other_entity = other_entity;
        normal = vec2(0.0);
    };
    Collision(Entity& other_entity, vec2 normal) {
        this->other_entity = other_entity;
        this->normal = normal;
    };

};

struct CollisionHandler{
    std::function<void(Entity, Collision)> onCollision = nullptr;
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};


// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

struct Scrolling {
    vec2 startPosition, endPosition;
    float speed = 0.1f;
    float alpha = 0.0f;
};

struct SpriteSheet {
    // Set to how many sprites are on the sheet horizontall and vertically
    // For example, if your sprite sheet is 3x4, set this to vec2(3,4)
    vec2 dimensions = vec2(1.0, 1.0);

    // Set this to which sprite on the sheet you would like to display
    // For example, if you want sprite at 1x2 to appear, set this to vec2(0,1)
    vec2 current = vec2(0.0, 0.0);

    // Controls how fast each frame is animated
    // Frames are animated from top-left to bottom-right
    // Set to 0 to disable animations
    float frameSpeed = 0.0f;

    float nextFrame = 0.0f;

	bool looping = true;
};

struct CleanTool {
    float shakeTime = 0.0f;
	float timer = 4.f;
};

struct Poop
{

};


struct Toilet {

};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example Textures are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class Textures {
	TEST_GUY,
    LIVING_ROOM,
    KITCHEN_ROOM,
    MAIL_ICON,
    MAIL_NOTIF_ICON,
    POSTCARD,
	FRIDGE,
	FRIDGE_OPEN,
	PAN,
	PANCAKE,
	PLATE,
	CLOSE,
	BACKYARD,
	SEED_PLOT,
	CARROT,
	PUMPKIN,
	BANANA,
	MELON,
	BEDROOM,
	DREAM_BUBBLE,
	SHEEP1,
	DREAM_GAME_GROUND,
	BATH_ROOM,
	GUY_RUN_SPRITESHEET,
	GUY_SLEEP_SPRITESHEET,
	GUY_JUMP,
	GUY_DEATH,
	SHEEP_SPRITESHEET,
	ENTERTAINMENT,
	HYGIENE,
	REST,
	SATIATION,
	STAR,
    BASKET,
	TOILET,
	POOP,
	BLUESKY1,
	BLUESKY2,
	BLUESKY3,
	BLUESKY4,
	MOUNTAIN1,
	MOUNTAIN2,
	MOUNTAIN3,
	NIGHTSKY1,
	NIGHTSKY2,
	NIGHTSKY3,
	GIFTBOX_OPEN,
	GIFTBOX_CLOSE,
	BATHROOM_GAME_BACKGROUND,
	BATHROMM_GAME_CLOUD,
	SOAP,
	RABBIT1,
	RABBIT2,
	RABBIT_SPRITESHEET,
	TWISKY1,
	TWISKY2,
	TWISKY3,
	TWISKY4,
	TWISKY5,
	DAWNSKY1,
	DAWNSKY2,
	DAWNSKY3,
	DAWNSKY4,
	LAMP,
	TREE_DOOR,
	GUN,
	BULLET,
	SQUIRREL_1,
	SQUIRREL_2,
	SQUIRREL_SPRITESHEET,
	SQUIRREL_HIT,
	BACKYARD_GAME_BACKGROUND,
	TUTSKY1,
	TUTSKY2,
	TUTBACKGROUND,
	EGG_IDLE,
	CHATBOX,
	EGG_HATCH,
	CHATBOX_SPRITESHEET,
	FERTILIZER,
	FERTILIZER_SHADOW,
    LEFT_KEY,
    RIGHT_KEY,
    WHISTLE,

    BUDDY_BABY_DEATH,
    BUDDY_BABY_IDLE,
    BUDDY_BABY_JUMP,
    BUDDY_BABY_RUN,
    BUDDY_BABY_SLEEP,

    BUDDY_SMART_DEATH,
    BUDDY_SMART_IDLE,
    BUDDY_SMART_JUMP,
    BUDDY_SMART_RUN,
    BUDDY_SMART_SLEEP,

    BUDDY_CUTE_DEATH,
    BUDDY_CUTE_IDLE,
    BUDDY_CUTE_JUMP,
    BUDDY_CUTE_RUN,
    BUDDY_CUTE_SLEEP,

    BUDDY_JOCK_DEATH,
    BUDDY_JOCK_IDLE,
    BUDDY_JOCK_JUMP,
    BUDDY_JOCK_RUN,
    BUDDY_JOCK_SLEEP,

    BUDDY_COOL_DEATH,
    BUDDY_COOL_IDLE,
    BUDDY_COOL_JUMP,
    BUDDY_COOL_RUN,
    BUDDY_COOL_SLEEP,

    BUDDY_GOLD_DEATH,
    BUDDY_GOLD_IDLE,
    BUDDY_GOLD_JUMP,
    BUDDY_GOLD_RUN,
    BUDDY_GOLD_SLEEP,

    BUDDY_HOBO_DEATH,
    BUDDY_HOBO_IDLE,
    BUDDY_HOBO_JUMP,
    BUDDY_HOBO_RUN,
    BUDDY_HOBO_SLEEP,

    BUDDY_ADULT_SMART_DEATH,
    BUDDY_ADULT_SMART_IDLE,
    BUDDY_ADULT_SMART_JUMP,
    BUDDY_ADULT_SMART_RUN,
    BUDDY_ADULT_SMART_SLEEP,

    BUDDY_ADULT_CUTE_DEATH,
    BUDDY_ADULT_CUTE_IDLE,
    BUDDY_ADULT_CUTE_JUMP,
    BUDDY_ADULT_CUTE_RUN,
    BUDDY_ADULT_CUTE_SLEEP,

    BUDDY_ADULT_JOCK_DEATH,
    BUDDY_ADULT_JOCK_IDLE,
    BUDDY_ADULT_JOCK_JUMP,
    BUDDY_ADULT_JOCK_RUN,
    BUDDY_ADULT_JOCK_SLEEP,

    BUDDY_ADULT_COOL_DEATH,
    BUDDY_ADULT_COOL_IDLE,
    BUDDY_ADULT_COOL_JUMP,
    BUDDY_ADULT_COOL_RUN,
    BUDDY_ADULT_COOL_SLEEP,

    BUDDY_ADULT_GOLD_DEATH,
    BUDDY_ADULT_GOLD_IDLE,
    BUDDY_ADULT_GOLD_JUMP,
    BUDDY_ADULT_GOLD_RUN,
    BUDDY_ADULT_GOLD_SLEEP,

    BUDDY_ADULT_HOBO_DEATH,
    BUDDY_ADULT_HOBO_IDLE,
    BUDDY_ADULT_HOBO_JUMP,
    BUDDY_ADULT_HOBO_RUN,
    BUDDY_ADULT_HOBO_SLEEP,

    TEXTURE_COUNT,
};
const int texture_count = static_cast<int>(Textures::TEXTURE_COUNT);

enum class Effects {
    COLOURED,
    TEXTURED,
    POST_PROCESS,
	PARTICLES,
    TEXT_SHADER,
    EFFECT_COUNT
};
const int effect_count = (int)Effects::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SALMON = 0,
	SPRITE = SALMON + 1,
	PEBBLE = SPRITE + 1,
	SQUIRREL = PEBBLE + 1,
	DEBUG_LINE = SQUIRREL + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	GEOMETRY_COUNT = SCREEN_TRIANGLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	Textures used_texture = Textures::TEXTURE_COUNT;
	Effects used_effect = Effects::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
	int z_index = 0;

    std::unordered_map<std::string, int> iUniforms;
    std::unordered_map<std::string, float> fUniforms;
};
