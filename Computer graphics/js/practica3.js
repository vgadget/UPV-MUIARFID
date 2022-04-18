/*
                                                       
 ,----.   ,------.  ,-----.               ,------. ,----.  
'  .-./   |  .--. ''  .--./    ,-----.    |  .--. ''.-.  | 
|  | .---.|  '--' ||  |        '-----'    |  '--' |  .' <  
'  '--'  ||  | --' '  '--'\               |  | --' /'-'  | 
 `------' `--'      `-----'               `--'     `----'  
                                                           

2021/2022
(c) ADRIÁN VÁZQUEZ BARRERA

*/

const FOV = 75;
var cameraControls;

var windowHeight = window.innerHeight;
var windowWidth = window.innerWidth;
var aspectRatio = windowWidth / windowHeight;
var resized = false;

const { scene, camera, miniCamera, renderer } = setupScene(perspective = true);

var TARGET;

function main() {

    let material = new THREE.MeshBasicMaterial({
        color: "red",
        wireframe: true
    });

    let plano = generarPlano(material);

    scene.add(generarRobot(material));
    scene.add(plano);


    resized = true;
    refresh();
    animate();
}


function generarRobot(material) {
    let obj = new THREE.Mesh();

    let base = new THREE.Mesh(
        new THREE.CylinderGeometry(50, 50, 15, 25),
        material
    );

    let brazo = generarBrazoRobot(material);
    base.add(brazo);

    obj.add(base);


    obj.position.y = 0;

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
        new THREE.Face3(0, 6, 1),
        new THREE.Face3(1, 6, 3),
        new THREE.Face3(6, 7, 3),
        new THREE.Face3(3, 2, 4),
        new THREE.Face3(3, 4, 7),
        new THREE.Face3(3, 10, 11),
        new THREE.Face3(2, 10, 3),
        new THREE.Face3(2, 4, 10),
        new THREE.Face3(4, 8, 10),
        new THREE.Face3(10, 8, 11),
        new THREE.Face3(11, 8, 9),
        new THREE.Face3(3, 11, 7),
        new THREE.Face3(7, 9, 11)
    );

    let pinza = new THREE.Mesh(
        geometriaPinza,
        material
    );

    pinza.position.x = alto / 2;

    return pinza;
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
    obj.add(nervios);

    //Mano
    let mano = new THREE.Mesh(
        new THREE.CylinderGeometry(radio_mano, radio_mano, 40, 20),
        material
    );
    mano.rotation.z = -Math.PI / 2;
    mano.position.y = 80;
    obj.add(mano);

    //Pinzas
    let pinzaR = generarPinzaRobot(material);
    let pinzaL = generarPinzaRobot(material);

    pinzaR.rotation.z = -Math.PI / 2;
    pinzaL.rotation.z = -Math.PI / 2;

    pinzaR.position.y = distancia_nervios;
    pinzaL.position.y = -distancia_nervios;

    mano.add(pinzaR);
    mano.add(pinzaL);


    //temp
    //obj.rotation.z = Math.PI/2;

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
    obj.add(baseBrazo);

    let nervioBrazo = new THREE.Mesh(
        new THREE.BoxGeometry(18, 120, 12),
        material
    );
    nervioBrazo.position.y = baseBrazo.position.y + 3 * radio;
    obj.add(nervioBrazo);

    let esfera = new THREE.Mesh(
        new THREE.SphereGeometry(20, 20, 15),
        material
    );
    esfera.position.y = 120;
    obj.add(esfera);

    let antebrazo = generarAntebrazoRobot(material);
    antebrazo.position.y = 120;

    obj.add(antebrazo);

    return obj;
}

function generarPlano(material) {
    let plano = new THREE.Mesh(
        new THREE.PlaneGeometry(1000, 1000, 25, 25),
        material
    );

    plano.rotation.x = Math.PI / 2;
    plano.position.z = -200;

    return plano;
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
        camera.position.y = 230;
        camera.rotation.x = -Math.PI / 7;
    } else {
        camera = new THREE.OrthographicCamera(
            windowWidth / 4,
            windowWidth / -4,
            windowHeight / 4,
            windowHeight / -4,
            1,
            1000
        );
        camera.position.y = 100;
        camera.rotation.x = -Math.PI / 5;
    }
    camera.position.z = 300;

    //SETUP MINI CAMERA
    miniCamera = new THREE.PerspectiveCamera(
        FOV,
        aspectRatio
    );
    miniCamera.position.set(0, 270, 0);
    miniCamera.lookAt(new THREE.Vector3(0, 0, 0));
    

    let renderer = new THREE.WebGLRenderer({ alpha: true });
    renderer.setSize(windowWidth, windowHeight);
    document.getElementById("canvas").appendChild(renderer.domElement);

    // Look at target
    camera.lookAt(new THREE.Vector3(0, 0, 0));
    cameraControls = new THREE.OrbitControls(camera, renderer.domElement);
    cameraControls.target.set(0, 0, 0)

    //Add window event listener
    // resize event listener
    window.addEventListener('resize', function () {
        resized = true
    });

    return { scene, camera, miniCamera, renderer };
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
    renderer.setScissorTest( true );
    let L = Math.min(windowHeight, windowWidth)/4;
    
    let mcH = L*(1/aspectRatio);
    let mcW = L;
    
    if (aspectRatio < 1)
    {
        mcH = L;
        mcW = L*(aspectRatio);
    }

    renderer.setScissor( 0, 0, mcW, mcH);
    renderer.setViewport(0, 0, mcW, mcH);
    renderer.render(scene, miniCamera);

    renderer.setScissorTest( false );

}

function animate() {

    requestAnimationFrame(animate);

    if (TARGET) {
        TARGET.rotation.y += 0.015;
    }
    refresh();
}