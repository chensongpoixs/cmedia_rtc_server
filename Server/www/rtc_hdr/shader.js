//const video = document.getElementById('video');
//
//navigator.mediaDevices.getUserMedia({ video: true })
//    .then((stream) => {
//        video.srcObject = stream;
//    })
//    .catch((error) => {
//        console.error('Error accessing webcam:', error);
//    });
//	
//	
//	
//	const canvas = document.getElementById('canvas');
//const gl = canvas.getContext('webgl') || canvas.getContext('experimental-webgl');
//
//
//const vertexShaderSource = `
//    attribute vec4 a_position;
//    attribute vec2 a_texcoord;
//    varying vec2 v_texcoord;
//    
//    void main() {
//        
//		 
//        v_texcoord =  a_texcoord ;
//		// v_texcoord.y   *=    -1     ; 
//		 gl_Position = vec4(a_position.x, -a_position.y, a_position.zw);;
//    }
//`;
//
//const fragmentShaderSource = `
//    precision mediump float;
//    uniform sampler2D u_texture;
//    varying vec2 v_texcoord;
//
//    void main() {
//        vec4 texColor = texture2D(u_texture, v_texcoord);
//        // Apply your shader effect here
//        gl_FragColor = texColor; // Example: pass-through shader
//    }
//`;
//function createShader(gl, sourceCode, type) 
//{
//    const shader = gl.createShader(type);
//    gl.shaderSource(shader, sourceCode);
//    gl.compileShader(shader);
//    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
//        console.error('Shader compilation error:', gl.getShaderInfoLog(shader));
//        gl.deleteShader(shader);
//        return null;
//    }
//    return shader;
//}
//
//const vertexShader = createShader(gl, vertexShaderSource, gl.VERTEX_SHADER);
//const fragmentShader = createShader(gl, fragmentShaderSource, gl.FRAGMENT_SHADER);
//
//const shaderProgram = gl.createProgram();
//gl.attachShader(shaderProgram, vertexShader);
//gl.attachShader(shaderProgram, fragmentShader);
//gl.linkProgram(shaderProgram);
//
//if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
//    console.error('Shader program linking error:', gl.getProgramInfoLog(shaderProgram));
//}
//const positionLocation = gl.getAttribLocation(shaderProgram, 'a_position');
//const texcoordLocation = gl.getAttribLocation(shaderProgram, 'a_texcoord');
//const textureLocation = gl.getUniformLocation(shaderProgram, 'u_texture');
//
//const positionBuffer = gl.createBuffer();
//gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
//// Define a rectangle that covers the canvas
//const positions = new Float32Array([
//    -1, -1,
//    -1, 1,
//    1, -1,
//    1, 1,
//]);
//gl.bufferData(gl.ARRAY_BUFFER, positions, gl.STATIC_DRAW);
//
//const texcoordBuffer = gl.createBuffer();
//gl.bindBuffer(gl.ARRAY_BUFFER, texcoordBuffer);
//// Define texture coordinates for the rectangle
//const texcoords = new Float32Array([
//    0, 0,
//    0, 1,
//    1, 0,
//    1, 1,
//]);
//gl.bufferData(gl.ARRAY_BUFFER, texcoords, gl.STATIC_DRAW);
//
//gl.useProgram(shaderProgram);
//
//// Bind the position buffer
//gl.enableVertexAttribArray(positionLocation);
//gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
//gl.vertexAttribPointer(positionLocation, 2, gl.FLOAT, false, 0, 0);
//
//// Bind the texcoord buffer
//gl.enableVertexAttribArray(texcoordLocation);
//gl.bindBuffer(gl.ARRAY_BUFFER, texcoordBuffer);
//gl.vertexAttribPointer(texcoordLocation, 2, gl.FLOAT, false, 0, 0);
//
//// Set the texture uniform to 0 (the first texture unit)
//gl.uniform1i(textureLocation, 0);
//
//// Create a texture to hold the video frame
//const texture = gl.createTexture();
//gl.activeTexture(gl.TEXTURE0);
//gl.bindTexture(gl.TEXTURE_2D, texture);
//
//// Configure texture parameters (optional)
//gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
//gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
//gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
//gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
//
//// Update the texture with the video frame in a loop
//function updateTexture() {
//    gl.bindTexture(gl.TEXTURE_2D, texture);
//    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, video);
//    requestAnimationFrame(updateTexture);
//}
//updateTexture();
//function drawFrame() {
//    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
//    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4); // Draw the rectangle
//    requestAnimationFrame(drawFrame);
//}
//drawFrame();
//

const video = document.getElementById('video');
navigator.mediaDevices.getUserMedia({ video: true })
    .then((stream) => {
        video.srcObject = stream;
    })
    .catch((error) => {
        console.error('Error accessing webcam:', error);
    });
 
    // Wait for the document to be fully loaded
    document.addEventListener('DOMContentLoaded', function () {
        // Get video and canvas elements
       // const video = document.getElementById('video');
        const canvas = document.getElementById('canvas');
        const ctx = canvas.getContext('2d');

        // Initialize PixiJS application
        const app = new PIXI.Application({
            view: canvas,
            width: 640,
            height: 360,
            transparent: true,
        });

        // Create a PIXI video sprite
        const videoSprite = PIXI.Texture.from(video);
        const videoSpriteObj = new PIXI.Sprite(videoSprite);
        app.stage.addChild(videoSpriteObj);

        // Render loop
        app.ticker.add(() => {
            // Copy video frame to the PIXI video sprite
            videoSprite.update();

            // Apply a simple brightness enhancement (adjust this based on your algorithm)
            for (let i = 0; i < videoSpriteObj.texture.data.length; i += 4) {
                videoSpriteObj.texture.data[i] += 20;  // Increase brightness by 20
                videoSpriteObj.texture.data[i + 1] += 20;
                videoSpriteObj.texture.data[i + 2] += 20;
            }
        });

        // Play the video when the PIXI app starts
        app.start();
        video.play();
    });
 
