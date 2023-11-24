<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>HDR Video with Three.js</title>
    <script type="module">
        import * as THREE from 'https://threejs.org/build/three.module.js';
        import { GLTFLoader } from 'https://threejs.org/examples/jsm/loaders/GLTFLoader.js';

        let camera, scene, renderer, video, texture, hdrRenderPass;

        init();
        animate();

        function init() {
            // 创建场景
            scene = new THREE.Scene();

            // 创建摄像机
            camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
            camera.position.z = 5;

            // 创建渲染器
            renderer = new THREE.WebGLRenderer();
            renderer.setSize(window.innerWidth, window.innerHeight);
            document.body.appendChild(renderer.domElement);

            // 加载HDR视频
            video = document.createElement('video');
            video.src = 'path/to/your/hdr-video.mp4';
            video.crossOrigin = 'anonymous';
            video.loop = true;
            video.muted = true;
            video.play();

            // 创建视频纹理
            texture = new THREE.VideoTexture(video);
            texture.encoding = THREE.sRGBEncoding;

            // 创建材质
            const material = new THREE.MeshBasicMaterial({ map: texture });

            // 创建立方体
            const geometry = new THREE.BoxGeometry();
            const cube = new THREE.Mesh(geometry, material);

            // 将立方体添加到场景
            scene.add(cube);

            // 添加HDR渲染通道
            const params = {
                exposure: 1.0,
            };

            hdrRenderPass = new THREE.RenderPass(scene, camera);
            hdrRenderPass.clear = false;

            // 设置曝光
            const toneMappingPass = new THREE.ToneMappingPass(params.exposure);
            toneMappingPass.needsSwap = true;

            // 创建渲染通道
            const renderPass = new THREE.RenderPass(scene, camera);

            // 创建复合通道
            const finalPass = new THREE.ShaderPass(THREE.FXAAShader);

            // 启用抗锯齿
            finalPass.material.uniforms.resolution.value.set(1 / window.innerWidth, 1 / window.innerHeight);

            // 合成通道
            const compositePass = new THREE.ShaderPass(THREE.AdditiveBlendingShader, 'tDiffuse1');
            compositePass.uniforms['tDiffuse1'].value = toneMappingPass.renderTarget.texture;
            compositePass.uniforms['tDiffuse2'].value = renderPass.renderTarget.texture;

            // 将通道添加到合成通道
            const effectComposer = new THREE.EffectComposer(renderer);
            effectComposer.addPass(hdrRenderPass);
            effectComposer.addPass(renderPass);
            effectComposer.addPass(toneMappingPass);
            effectComposer.addPass(finalPass);
            effectComposer.addPass(compositePass);

            // 调整窗口大小
            window.addEventListener('resize', onWindowResize, false);

            // 更新视频纹理
            function update() {
                if (video.readyState === video.HAVE_ENOUGH_DATA) {
                    texture.needsUpdate = true;
                }
            }

            // 渲染循环
            function animate() {
                requestAnimationFrame(animate);
                update();
                render();
            }

            // 渲染场景
            function render() {
                // 渲染到HDR纹理
                effectComposer.render();

                // 输出到屏幕
                renderer.render(scene, camera);
            }

            // 调整窗口大小
            function onWindowResize() {
                camera.aspect = window.innerWidth / window.innerHeight;
                camera.updateProjectionMatrix();
                renderer.setSize(window.innerWidth, window.innerHeight);
                effectComposer.setSize(window.innerWidth, window.innerHeight);
            }
        }
    </script>
</head>
<body style="margin: 0;">
</body>
</html>
