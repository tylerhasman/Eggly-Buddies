#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float u_Time;



// Example for y'all
uniform int u_EnableExampleFunction = 0;
uniform float u_ExampleFloat = 0.0;
uniform int u_EnableParallax = 0;
uniform int u_EnableCleaning = 0;
uniform float u_ParallaxSpeed = 0.0;

// status bars
uniform int u_EnableBar = 0;
uniform float u_barFloat = 100.0;
uniform int u_barIndicator = 0;


// lightswitch
uniform int u_EnableDarken = 0;
uniform float u_DarkenFloat = 0.6;

//fade screen to black
uniform int u_EnableFadeToBlack = 0;
uniform float u_FadeToBlackFactor = 0.0;

// Plant growing data
uniform int u_EnableGrowth = 0;
uniform float u_InitialHeight = 0.0;
uniform float u_FinalHeight = 0.0;
uniform float u_GrowthTime = 0.0;
uniform float u_StartGrowthTime = 0.0;

uniform int u_EnableHardcore = 0;

uniform int u_EnableHoverGlow;

// Output color
layout(location = 0) out  vec4 color;

vec4 exampleShaderFunction(vec4 inColor){
    if(u_EnableExampleFunction == 1) {
        inColor.r *= u_ExampleFloat;
    }
    return inColor;
}

vec4 darken(vec4 in_color) 
{
	if(u_EnableDarken == 1) {
        in_color.rgb *= u_DarkenFloat;
    }
    return in_color;
}


vec2 applyParallax() {
    vec2 parallaxCoord = vec2(texcoord.x, texcoord.y);
    if (u_EnableParallax == 1) {
        float offset = mod(u_ParallaxSpeed * u_Time, 1.0); // Ensures the offset loops between 0 and 1
        parallaxCoord = vec2(texcoord.x + offset, texcoord.y);
    }
    return parallaxCoord;
}

vec4 fadeToBlack(vec4 in_color) 
{
    if(u_EnableFadeToBlack == 1) {
        if (u_FadeToBlackFactor > 0)
		    in_color -= u_FadeToBlackFactor * vec4(0.8, 0.8, 0.8, 0);
    }
	
	return in_color;
}


vec4 applyWashing(vec4 inColor) {
    if (u_EnableCleaning == 1) {
    // Blinking Effect
        float blinkValue = 0.5 * sin(u_Time * 15.0) + 0.5; // Oscillates between 0 and 1.

        inColor *= mix(vec4(1.0), vec4(2.0), blinkValue); // Mix between normal and bright based on blinkValue.
        
        // Random White Dots Effect
        if (fract(sin(dot(texcoord + vec2(u_Time * 0.0001, u_Time * 0.0001), vec2(23, 33))) * 3333.333) > 0.95) {
            inColor.rgb = vec3(1.0);
        }
        
        // Clamp values to ensure they are within [0, 1]
        inColor.rgb = clamp(inColor.rgb, 0.0, 1.0);
    
    }
    return inColor;
}

vec4 applyBar(vec4 inColor) {
    if (u_EnableBar == 1) {
        float gradientX = texcoord.x;
        float fillAmount = u_barFloat/100.f;
        fillAmount = clamp(fillAmount, 0.0, 1.0);
        float gradientPosition = step(gradientX, fillAmount);

        vec3 color; 
        float normalizedX =  0.0f;
        if (u_barIndicator == 1) color = vec3(1.0, 0.055, 0.0); // dirty
        if (u_barIndicator == 2) color = vec3(0.0, 0.4, 0.8); // tired
        if (u_barIndicator == 3) color = vec3(0.0, 0.5, 0.0);// hunger
        if (u_barIndicator == 4) color = vec3(1.0, 0.41, 0.76); //boredom        
        vec3 gradientColor = mix(vec3(0.2, 0.2, 0.2),color,gradientPosition);

        vec4 mixColor = vec4(gradientColor,1.0);
        inColor *= mixColor;
    }
    return inColor;
}

vec4 growFunction(vec4 inColor){
    if(u_EnableGrowth == 1) {
		float interpolate = min(1.0, ((u_Time * 10.0) - u_StartGrowthTime) / u_GrowthTime);
		float height = mix(1.0 - u_InitialHeight, 1.0 - u_FinalHeight, interpolate);
		if (texcoord.y > height) {
			float growth_y = texcoord.y;
			growth_y -= height;
			inColor = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, growth_y));
		} else {
			discard;
		}

    }
    return inColor;
}

vec3 hoverGlow(vec3 inColor){
    if(u_EnableHoverGlow == 1){
        return mix(inColor, vec3(1.0, 1.0, 0.0), 0.5);
    }
    return inColor;
}

float hardCore(float r){
    if(u_EnableHardcore == 1){
        return r * 1.5;
    }
    return r;
}

void main()
{

    vec2 currentTexCoord = applyParallax();

	color = vec4(fcolor, 1.0) * texture(sampler0, currentTexCoord);

    color = applyWashing(color);

    color = applyBar(color);
    
	// Chain each function one after the other
	color = darken(color);
	// color = exampleShaderFunction(color);
	//color *= applyParallax(color);
	color = fadeToBlack(color);


	color = exampleShaderFunction(color);
	color = growFunction(color);

	color.rgb = hoverGlow(color.rgb);

    color.r = hardCore(color.r);

	/*
	    Example:
	    color = shaderFunctionOne(color);
	    color = shaderFunctionTwo(color);

	*/

}
