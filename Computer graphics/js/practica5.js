/*
                                                                                                              
                                                           
 ,----.   ,------.  ,-----.               ,------. ,-----. 
'  .-./   |  .--. ''  .--./    ,-----.    |  .--. '|  .--' 
|  | .---.|  '--' ||  |        '-----'    |  '--' |'--. `\ 
'  '--'  ||  | --' '  '--'\               |  | --' .--'  / 
 `------' `--'      `-----'               `--'     `----'  
                                                                                                      

 2021/2022
(c) ADRIÁN VÁZQUEZ BARRERA

*/

//Camera settings
const FOV = 70;
var cameraControls;

//Window status
var windowHeight = window.innerHeight;
var windowWidth = window.innerWidth;
var aspectRatio = windowWidth / windowHeight;
var resized = false;

//Scene management
const { scene, camera, miniCamera, renderer } = setupScene(perspective = true);
const eventManager = new THREEx.DomEvents(camera, renderer.domElement); // Event in scene
const keyboardManager = new THREEx.KeyboardState(renderer.domElement); // Event from peripheral
const gui = setupGui();


//Animation settings
const FPS = 60;
const ROBOT_SPEED = 200;
var prevTime = null;
var updateQueue = [];

//Quick access to robot parts
const ROBOT = 0, ROBOT_ARM = 1, ROBOT_FOREARM = 2, ROBOT_HAND = 3, ROBOT_PLIER_L = 4, ROBOT_PLIER_R = 5;
const robotPieces = Array(6);

function main() {

    let material = new THREE.MeshBasicMaterial({
        color: "red",
        wireframe: false
    });

    let plano = generarPlano(material);

    scene.add(generarRobot(material));
    robotPieces[ROBOT].rotation.y = -Math.PI / 4;

    scene.add(plano);
    genearLuz();

    resized = true;
    refresh();
    requestAnimationFrame(animate);
    setupInputs();
}

function genearLuz() {

    let luzAmbiental = new THREE.AmbientLight("white", 0.7);
    scene.add(luzAmbiental);

    let luzDireccional = new THREE.DirectionalLight("white", 0.5);
    luzDireccional.position.set(0, 1, -3);
    scene.add(luzDireccional);

    let luzPuntual = new THREE.PointLight("blue", 15, 150);
    luzPuntual.position.set(-450, 40, -500);
    scene.add(luzPuntual);

    let luzFocal = new THREE.SpotLight("white", 2, 1200);
    luzFocal.position.set(800, 800, 600);
    luzFocal.target.position.set(0, 0, 0);
    luzFocal.angle = Math.PI / 10;
    luzFocal.penumbra = 1;
    luzFocal.shadow.camera.near = 5;
    luzFocal.shadow.camera.far = 100000;
    luzFocal.shadow.camera.visible = true;
    luzFocal.castShadow = true;

    let luzFocal2 = new THREE.SpotLight("white", 1, 1800);
    luzFocal2.position.set(-500, 800, 600);
    luzFocal2.target.position.set(0, 0, 0);
    luzFocal2.angle = Math.PI / 10;
    luzFocal2.penumbra = 1;
    luzFocal2.shadow.camera.near = 5;
    luzFocal2.shadow.camera.far = 100000;
    luzFocal2.shadow.camera.visible = true;
    luzFocal2.castShadow = true;

    scene.add(luzFocal);
    scene.add(luzFocal2);


}

function generarRobot(material) {

    let obj = new THREE.Mesh();

    // load a texture, set wrap mode to repeat
    const texture = new THREE.TextureLoader().load("../images/metal_128x128.jpg");
    texture.wrapS = THREE.RepeatWrapping;
    texture.wrapT = THREE.RepeatWrapping;
    texture.repeat.set(1, 1);

    let materialMetal = new THREE.MeshLambertMaterial({
        color: "white",
        specular: "white",
        shininess: 15,
        map: texture,
        wireframe: false
    });

    let base = new THREE.Mesh(
        new THREE.CylinderGeometry(50, 50, 15, 25),
        materialMetal
    );

    base.receiveShadow = true;
    base.castShadow = true;

    let brazo = generarBrazoRobot(materialMetal);
    base.add(brazo);
    robotPieces[ROBOT_ARM] = brazo;


    obj.add(base);

    robotPieces[ROBOT] = obj;

    obj.castShadow = true;

    return obj;
}

function generarPinzaRobot(material) {

    let longitudPieza = 38;
    let alto = 20;
    let alto2 = 15;
    let ancho = 19;
    let grosor = 4;
    let grosor2 = 2;

    let geometriaPinza = new THREE.Geometry();

    let v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12;

    v1 = new THREE.Vector3(0, 0, 0);
    v2 = new THREE.Vector3(grosor, 0, 0);
    v3 = new THREE.Vector3(0, 0, ancho);
    v4 = new THREE.Vector3(grosor, 0, ancho);
    v5 = new THREE.Vector3(0, -alto, ancho);
    v6 = new THREE.Vector3(0, -alto, 0);
    v7 = new THREE.Vector3(grosor, -alto, 0);
    v8 = new THREE.Vector3(grosor, -alto, ancho);

    let difAltura = Math.abs(alto - alto2);
    v9 = new THREE.Vector3(0, -alto2 - difAltura / 2, longitudPieza);
    v10 = new THREE.Vector3(grosor2, -alto2 - difAltura / 2, longitudPieza);
    v11 = new THREE.Vector3(0, -difAltura / 2, longitudPieza);
    v12 = new THREE.Vector3(grosor2, -difAltura / 2, longitudPieza);



    geometriaPinza.vertices.push(
        v1, // 0 
        v2, // 1
        v3, // 2
        v4, // 3
        v5, // 4
        v6, // 5
        v7, // 6
        v8, // 7
        v9, // 8
        v10, // 9
        v11, // 10
        v12, // 11
    );

    geometriaPinza.faces.push(
        new THREE.Face3(0, 2, 1),
        new THREE.Face3(1, 2, 3),
        new THREE.Face3(0, 5, 2),
        new THREE.Face3(5, 4, 2),
        new THREE.Face3(0, 5, 6),
        new THREE.Face3(5, 6, 4),
        new THREE.Face3(4, 6, 7),
        new THREE.Face3(0, 6, 1),
        new THREE.Face3(1, 6, 3),
        new THREE.Face3(6, 7, 3),
        new THREE.Face3(3, 2, 4),
        new THREE.Face3(3, 4, 7),
        new THREE.Face3(3, 10, 11),
        new THREE.Face3(2, 10, 3),
        new THREE.Face3(2, 4, 10),
        new THREE.Face3(4, 8, 10),
        new THREE.Face3(4, 7, 8),
        new THREE.Face3(7, 9, 8),
        new THREE.Face3(10, 8, 11),
        new THREE.Face3(11, 8, 9),
        new THREE.Face3(3, 11, 7),
        new THREE.Face3(7, 9, 11)
    );

    // load a texture, set wrap mode to repeat
    const texture = new THREE.TextureLoader().load("../images/metal_128x128.jpg");
    texture.wrapS = THREE.RepeatWrapping;
    texture.wrapT = THREE.RepeatWrapping;
    texture.repeat.set(1, 1);

    let materialMetal = new THREE.MeshLambertMaterial({
        color: "white",
        specular: "white",
        shininess: 15,
        map: texture,
        wireframe: false
    });



    let pinza = new THREE.Mesh(
        geometriaPinza,
        materialMetal
    );

    geometriaPinza.computeFaceNormals();
    geometriaPinza.computeVertexNormals();
    materialMetal.side = THREE.DoubleSide;

    pinza.position.x = -grosor / 2;
    pinza.position.y = alto / 2;

    pinza.receiveShadow = true;
    pinza.castShadow = true;

    let obj = new THREE.Mesh();
    obj.add(pinza);

    return obj;
}

function generarAntebrazoRobot(material) {

    let radio_disco = 22;
    let radio_mano = 15;
    let distancia_nervios = radio_disco / 3;


    let obj = new THREE.Mesh();

    //Disco
    let disco = new THREE.Mesh(
        new THREE.CylinderGeometry(radio_disco, radio_disco, 6, 20),
        material
    );
    disco.receiveShadow = true;
    disco.castShadow = true;
    obj.add(disco);


    //Nervios
    let nervios = new THREE.Mesh();
    let nerv1 = new THREE.Mesh(
        new THREE.BoxGeometry(4, 80, 4),
        material
    );
    let nerv2 = new THREE.Mesh(
        new THREE.BoxGeometry(4, 80, 4),
        material
    );
    let nerv3 = new THREE.Mesh(
        new THREE.BoxGeometry(4, 80, 4),
        material
    );
    let nerv4 = new THREE.Mesh(
        new THREE.BoxGeometry(4, 80, 4),
        material
    );
    nerv1.position.x = distancia_nervios;
    nerv2.position.x = -distancia_nervios;
    nerv3.position.x = distancia_nervios;
    nerv4.position.x = -distancia_nervios;
    nerv1.position.z = distancia_nervios;
    nerv2.position.z = distancia_nervios;
    nerv3.position.z = -distancia_nervios;
    nerv4.position.z = -distancia_nervios;
    nervios.add(nerv1);
    nervios.add(nerv2);
    nervios.add(nerv3);
    nervios.add(nerv4);
    nervios.position.y = nerv1.geometry.parameters.height / 2;
    nerv1.receiveShadow = true;
    nerv1.castShadow = true;
    nerv2.receiveShadow = true;
    nerv2.castShadow = true;
    nerv3.receiveShadow = true;
    nerv3.castShadow = true;
    nerv4.receiveShadow = true;
    nerv4.castShadow = true;
    obj.add(nervios);

    //Mano
    let mano = new THREE.Mesh(
        new THREE.CylinderGeometry(radio_mano, radio_mano, 40, 20),
        material
    );
    mano.rotation.z = -Math.PI / 2;
    mano.position.y = 80;
    mano.receiveShadow = true;
    mano.castShadow = true;
    obj.add(mano);
    robotPieces[ROBOT_HAND] = mano;

    //Pinzas
    let pinzaR = generarPinzaRobot(material);
    let pinzaL = generarPinzaRobot(material);

    pinzaR.rotation.z = -Math.PI / 2;
    pinzaL.rotation.z = -Math.PI / 2;

    pinzaR.position.y = distancia_nervios;
    pinzaL.position.y = -distancia_nervios;

    mano.add(pinzaR);
    mano.add(pinzaL);

    robotPieces[ROBOT_PLIER_L] = pinzaL;
    robotPieces[ROBOT_PLIER_R] = pinzaR;

    return obj;
}

function generarBrazoRobot(material) {

    let radio = 20;

    let obj = new THREE.Mesh();

    let baseBrazo = new THREE.Mesh(
        new THREE.CylinderGeometry(radio, radio, 18, 25),
        material
    );
    baseBrazo.rotation.x = -Math.PI / 2;
    baseBrazo.rotation.z = -Math.PI / 2;
    baseBrazo.receiveShadow = true;
    baseBrazo.castShadow = true;
    obj.add(baseBrazo);

    let nervioBrazo = new THREE.Mesh(
        new THREE.BoxGeometry(18, 120, 12),
        material
    );
    nervioBrazo.position.y = baseBrazo.position.y + 3 * radio;
    nervioBrazo.receiveShadow = true;
    nervioBrazo.castShadow = true;
    obj.add(nervioBrazo);


    // load a texture, set wrap mode to repeat
    const mapaEntornoText = new THREE.CubeTextureLoader().load(
        [
            '../images/posx.jpg', '../images/negx.jpg',
            '../images/posy.jpg', '../images/negy.jpg',
            '../images/posz.jpg', '../images/negz.jpg',
        ]
    );
    mapaEntornoText.wrapS = THREE.RepeatWrapping;
    mapaEntornoText.wrapT = THREE.RepeatWrapping;
    mapaEntornoText.repeat.set(1, 1);

    let materialBrillante = new THREE.MeshPhongMaterial({
        color: "yellow",
        specular: "yellow",
        shininess: 80,
        envMap: mapaEntornoText,
        wireframe: false
    });

    let esfera = new THREE.Mesh(
        new THREE.SphereGeometry(20, 20, 15),
        materialBrillante
    );
    esfera.position.y = 120;
    esfera.receiveShadow = true;
    esfera.castShadow = true;
    obj.add(esfera);

    // load a texture, set wrap mode to repeat
    const texture = new THREE.TextureLoader().load("../images/wood512.jpg");
    texture.wrapS = THREE.RepeatWrapping;
    texture.wrapT = THREE.RepeatWrapping;
    texture.repeat.set(1, 1);

    let materialOro = new THREE.MeshLambertMaterial({
        color: "yellow",
        specular: "yellow",
        shininess: 1,
        map: texture,
        wireframe: false
    });

    let antebrazo = generarAntebrazoRobot(materialOro);
    antebrazo.position.y = 120;

    obj.add(antebrazo);
    robotPieces[ROBOT_FOREARM] = antebrazo;

    return obj;
}

function generarPlano(material) {

    let obj = new THREE.Mesh();

    // load a texture, set wrap mode to repeat
    const texture = new THREE.TextureLoader().load("../images/wood.jpg");
    texture.wrapS = THREE.RepeatWrapping;
    texture.wrapT = THREE.RepeatWrapping;
    texture.repeat.set(4, 4);

    let materialPlano = new THREE.MeshPhongMaterial({
        color: "white",
        specular: "white",
        shininess: 15,
        map: texture,
        wireframe: false
    });

    let plano = new THREE.Mesh(
        new THREE.PlaneGeometry(2000, 2000, 25, 25),
        materialPlano
    );

    plano.rotation.x = -Math.PI / 2;
    plano.position.z = -200;

    plano.receiveShadow = true;
    plano.castShadow = true;

    // PAREDES
    // load a texture, set wrap mode to repeat
    const textureP1 = new THREE.TextureLoader().load("../images/posx.jpg");
    textureP1.wrapS = THREE.RepeatWrapping;
    textureP1.wrapT = THREE.RepeatWrapping;
    textureP1.repeat.set(1, 1);

    let materialPared1 = new THREE.MeshPhongMaterial({
        color: "white",
        specular: "white",
        shininess: 15,
        map: textureP1,
        wireframe: false
    });

    let pared1 = new THREE.Mesh(
        new THREE.PlaneGeometry(2000, 2000, 25, 25),
        materialPared1
    );

    pared1.position.z = -1200;
    pared1.position.y = 0;
    pared1.receiveShadow = true;
    pared1.castShadow = true;

    const textureP3 = new THREE.TextureLoader().load("../images/posz.jpg");
    textureP3.wrapS = THREE.RepeatWrapping;
    textureP3.wrapT = THREE.RepeatWrapping;
    textureP3.repeat.set(1, 1);

    let materialPared3 = new THREE.MeshPhongMaterial({
        color: "white",
        specular: "white",
        shininess: 15,
        map: textureP3,
        wireframe: false
    });


    let pared3 = new THREE.Mesh(
        new THREE.PlaneGeometry(2000, 2000, 25, 25),
        materialPared3
    );
    pared3.position.z = -200;
    pared3.position.y = 0;
    pared3.position.x = -1000;
    pared3.rotation.y = Math.PI / 2;
    pared3.receiveShadow = true;
    pared3.castShadow = true;

    obj.add(plano);
    obj.add(pared1);
    obj.add(pared3);

    obj.rotation.y = -35 * (Math.PI / 180);

    return obj;
}

function setupScene(perspective = true) {

    let scene = new THREE.Scene();

    let camera = null;
    let miniCamera = null;

    //SETUP MAIN CAMERA
    if (perspective) {
        camera = new THREE.PerspectiveCamera(
            FOV,
            aspectRatio
        );
        camera.position.y = 180;
        camera.rotation.y = Math.PI / 4;
        camera.position.x = 40;
        camera.rotation.x = 0;
    } else {
        camera = new THREE.OrthographicCamera(
            windowWidth / 4,
            windowWidth / -4,
            windowHeight / 4,
            windowHeight / -4,
            1,
            1000
        );
        camera.position.y = 80;
        camera.rotation.x = -Math.PI / 6;
    }
    camera.position.z = 400;

    //SETUP MINI CAMERA
    miniCamera = new THREE.PerspectiveCamera(
        FOV,
        aspectRatio
    );
    miniCamera.position.set(0, 280, 15);
    miniCamera.lookAt(new THREE.Vector3(0, 0, 0));


    let renderer = new THREE.WebGLRenderer({ alpha: true });
    renderer.setSize(windowWidth, windowHeight);
    renderer.shadowMapEnabled = true;
    document.getElementById("canvas").appendChild(renderer.domElement);

    // Look at target
    camera.lookAt(new THREE.Vector3(0, 400, 0));
    cameraControls = new THREE.OrbitControls(camera, renderer.domElement);
    cameraControls.target.set(0, 0, 0);

    //Add window event listener
    // resize event listener
    window.addEventListener('resize', function () {
        resized = true
    });

    return { scene, camera, miniCamera, renderer };
}

function setupGui() {

    let gui = new dat.GUI({
        autoPlace: false,
        width: 300
    });

    // Controls definition
    controls = {
        giroBase: 0.0,
        giroBrazo: 0.00,
        giroAntebrazoY: 0.0,
        giroAntebrazoZ: 0.0,
        giroPinza: 0.0,
        separacionPinza: 0.0,
    }

    let menu = gui.addFolder("Control Robot");
    let menuGiroBase = menu.add(controls, "giroBase", -180.0, 180.0, 0.025).name("Giro Base").listen();
    let menuGiroBrazo = menu.add(controls, "giroBrazo", -45.0, 45.0, 0.025).name("Giro Brazo").listen();
    let menuGiroAntebrazoY = menu.add(controls, "giroAntebrazoY", -180.0, 180.0, 0.025).name("Giro Antebrazo Y").listen();
    let menuGiroAntebrazoZ = menu.add(controls, "giroAntebrazoZ", -90.0, 90.0, 0.025).name("Giro Antebrazo Z").listen();
    let menuGiroPinza = menu.add(controls, "giroPinza", -40.0, 220.0, 0.025).name("Giro Pinza").listen();
    let menuSeparacionPinza = menu.add(controls, "separacionPinza", 0, 15, 0.025).name("Separación pinza").listen();
    menu.open();

    menuGiroBase.onChange((value) => {
        robotPieces[ROBOT].rotation.y = (value * 2 * Math.PI) / 360;
    });

    menuGiroBrazo.onChange((value) => {
        robotPieces[ROBOT_ARM].rotation.x = (value * 2 * Math.PI) / 360;
    });

    menuGiroAntebrazoY.onChange((value) => {
        robotPieces[ROBOT_FOREARM].rotation.y = (value * 2 * Math.PI) / 360;
    });

    menuGiroAntebrazoZ.onChange((value) => {
        robotPieces[ROBOT_FOREARM].rotation.x = (value * 2 * Math.PI) / 360;
    });

    menuGiroPinza.onChange((value) => {
        robotPieces[ROBOT_HAND].rotation.x = -(value * 2 * Math.PI) / 360;
    });

    menuSeparacionPinza.onChange((value) => {
        robotPieces[ROBOT_PLIER_R].position.y = value;
        robotPieces[ROBOT_PLIER_L].position.y = -value;
    });

    document.getElementById("menu").appendChild(gui.domElement);

    return gui;
}

function setupInputs() {


    updateQueue.push((delta, now) => {

        if (keyboardManager.pressed("left")) {

            robotPieces[ROBOT].position.x -= ROBOT_SPEED * delta;
            robotPieces[ROBOT].rotation.y = -Math.PI / 2;

        } else if (keyboardManager.pressed("right")) {

            robotPieces[ROBOT].position.x += ROBOT_SPEED * delta;
            robotPieces[ROBOT].rotation.y = Math.PI / 2;

        }
        else if (keyboardManager.pressed("up")) {

            robotPieces[ROBOT].position.z -= ROBOT_SPEED * delta;
            robotPieces[ROBOT].rotation.y = Math.PI;

        }

        else if (keyboardManager.pressed("down")) {

            robotPieces[ROBOT].position.z += ROBOT_SPEED * delta;
            robotPieces[ROBOT].rotation.y = 0;

        }


    });

}

function resize() {

    resized = false;

    windowWidth = window.innerWidth * (95 / 100);
    windowHeight = window.innerHeight * (85 / 100);

    renderer.setSize(windowWidth, windowHeight, true);

    aspectRatio = windowWidth / windowHeight
    camera.aspect = aspectRatio;
    miniCamera.aspect = aspectRatio;
    camera.updateProjectionMatrix();
    miniCamera.updateProjectionMatrix();
}

function refresh() {

    if (resized) { resize() }

    //Clear
    renderer.setClearColor(0xffffff);
    renderer.clear();

    //Render main camera
    renderer.setViewport(0, 0, windowWidth, windowHeight);
    renderer.render(scene, camera);

    //Render mini camera
    renderer.clearDepth()
    renderer.setScissorTest(true);
    let L = Math.min(windowHeight, windowWidth) / 4;

    let mcH = L * (1 / aspectRatio);
    let mcW = L;

    if (aspectRatio < 1) {
        mcH = L;
        mcW = L * (aspectRatio);
    }

    renderer.setScissor(0, 0, mcW, mcH);
    renderer.setViewport(0, 0, mcW, mcH);
    renderer.render(scene, miniCamera);

    renderer.setScissorTest(false);

}

function animate(now) {

    prevTime = prevTime || now - 1000 / FPS;
    let deltaTime = Math.min(200, now - prevTime);
    prevTime = now;

    updateQueue.forEach((fn) => {
        fn(deltaTime / 1000, now / 1000);
    });

    refresh();
    renderer.domElement.setAttribute("tabIndex", "0");
    renderer.domElement.focus();
    requestAnimationFrame(animate);
}
