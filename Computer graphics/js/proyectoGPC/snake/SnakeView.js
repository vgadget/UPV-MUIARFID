/*
                                                                                                 
 ,----.   ,------.  ,-----.               ,------.                                     ,--.          
'  .-./   |  .--. ''  .--./    ,-----.    |  .--. ',--.--. ,---.,--. ,--.,---.  ,---.,-'  '-. ,---.  
|  | .---.|  '--' ||  |        '-----'    |  '--' ||  .--'| .-. |\  '  /| .-. :| .--''-.  .-'| .-. | 
'  '--'  ||  | --' '  '--'\               |  | --' |  |   ' '-' ' \   ' \   --.\ `--.  |  |  ' '-' ' 
 `------' `--'      `-----'               `--'     `--'    `---'.-'  /   `----' `---'  `--'   `---'  
                                                                `---'                                


                                                                
2021/2022
(c) ADRIÁN VÁZQUEZ BARRERA                                                            

*/

// Video variables
var cameraControls;
var windowHeight = window.innerHeight;
var windowWidth = (windowHeight) * (ASPECT_RATIO);
var resized = true;

//Scene stuff
const { scene, camera, renderer } = setupScene();
const loader = new THREE.GLTFLoader();

const snakeHead = new THREE.Mesh();

var appleModel = false;
var pointLight = new THREE.PointLight("orange", 2, 200);

loader.load('../models/apple/scene.gltf', function (gltf) {
    let s = gltf.scene;
    s.receiveShadow = true;
    s.castShadow = true;
    s.scale.set(0.25, 0.25, 0.25);
    s.position.x = 0;
    s.position.y = 0;
    s.position.z = -4.5;
    s.rotation.x = Math.PI / 2;
    appleModel = s;
});


//Game stuff
const GROUND_SIZE = BOARD_SIZE * BOARD_SIZE;
var snakeModel = new SnakeModel(BOARD_SIZE);
var { obj, board } = generateGround();
var snakeSpeed = initialSnakeSpeed;

//Animation stuff
var xRotationTarget = 0;
var yRotationTarget = 0;
var performingAnimation = false;

function main() {

    document.getElementById("gameover").hidden = true;

    let head = new THREE.Mesh(
        new THREE.ConeGeometry(8, 20, 10),
        new THREE.MeshBasicMaterial({ color: MAIN_COLOR })
    );
    head.position.y += 3;
    snakeHead.add(head);


    let generalLight = new THREE.AmbientLight("white", 0.80);
    scene.add(generalLight);

    let directionalLight1 = new THREE.DirectionalLight("white", 0.1);
    directionalLight1.position.set(-0.4, 0, 0.5);
    directionalLight1.rotation.y = Math.PI / 4;
    directionalLight1.shadow.camera.visible = true;
    directionalLight1.castShadow = true;
    scene.add(directionalLight1);

    let directionalLight2 = new THREE.DirectionalLight("white", 0.1);
    directionalLight2.position.set(0.4, 0, 0.5);
    directionalLight2.rotation.y = Math.PI / 4;
    directionalLight2.shadow.camera.visible = true;
    directionalLight2.castShadow = true;
    scene.add(directionalLight2);

    pointLight.position.z = -5;

    const video = document.getElementById('background');
    video.autoplay = true;
    video.load();
    video.play();
    const videoTexture = new THREE.VideoTexture(video);
    videoTexture.wrapS = videoTexture.wrapT = THREE.ClampToEdgeWrapping;
    videoTexture.minFilter = THREE.LinearFilter;

    let material = new THREE.MeshBasicMaterial({ map: videoTexture });

    let plano = new THREE.Mesh(
        new THREE.PlaneGeometry(1920, 1080, 25, 25),
        material
    );
    video.hidden = true;
    plano.position.z = -350;
    scene.add(plano);


    scene.add(obj);
    obj.position.z -= 50;

    snakeModel.direction = RIGHT;

    snakeModel.observers.push(lookAtSnake);
    snakeModel.observers.push(checkDeath);

    loadEventListeners();
    animate();
    startMove();
}

function updateBoard() {

    let tail_count = 0;

    for (let i = 0; i < board.length; i++) {
        for (let j = 0; j < board[i].length; j++) {
            for (let k = 0; k < board[i][j].length; k++) {

                //Reset to defaults each cube
                board[i][j][k].material.color.setHex(0xffffff);
                board[i][j][k].material.opacity = 1;
                board[i][j][k].material.transparent = false;

                for (let l = board[i][j][k].children.length - 1; l >= 0; l--) {
                    board[i][j][k].remove(board[i][j][k].children[l]);
                }

                switch (snakeModel.maze[i][j][k]) {

                    case EMPTY:
                        board[i][j][k].material.opacity = 0;
                        board[i][j][k].material.transparent = true;
                        break;
                    case PLAYER:
                        board[i][j][k].material.opacity = 0;
                        board[i][j][k].material.transparent = true;
                        board[i][j][k].add(snakeHead);

                        switch (snakeModel.direction) {

                            case RIGHT:
                                snakeHead.rotation.z = -Math.PI / 2;
                                break;
                            case LEFT:
                                snakeHead.rotation.z = Math.PI / 2;
                                break;
                            case UP:
                                snakeHead.rotation.z = 0;
                                break;
                            case DOWN:
                                snakeHead.rotation.z = Math.PI;
                                break;
                            default:
                                snakeHead.rotation.z = 0;
                                break;
                        }

                        break;
                    case TAIL:

                        if (tail_count % 2 == 0) {
                            board[i][j][k].material.color.setHex(SECONDARY_COLOR);
                        } else {
                            board[i][j][k].material.color.setHex(TERTIARY_COLOR);
                        }

                        tail_count++;
                        break;
                    case FOOD:
                        board[i][j][k].material.color.setHex(0xf50000);

                        if (appleModel !== false) {
                            board[i][j][k].material.opacity = 0;
                            board[i][j][k].material.transparent = true;
                            board[i][j][k].add(appleModel.clone());
                            board[i][j][k].add(pointLight.clone());
                        }


                        break;
                }
            }
        }
    }
}

function generateGround() {

    const texture = new THREE.TextureLoader().load("../images/proyectoGPC/ground.jpg");
    texture.wrapS = THREE.RepeatWrapping;
    texture.wrapT = THREE.RepeatWrapping;
    texture.repeat.set(1, 1);

    let material = new THREE.MeshPhongMaterial({
        color: "white",
        specular: "white",
        shininess: 30,
        map: texture,
        wireframe: false
    });


    let board = Array(6);

    //Main cube, other obj orbits
    let obj = new THREE.Mesh(
        new THREE.BoxGeometry(GROUND_SIZE, GROUND_SIZE, GROUND_SIZE),
        material || new THREE.MeshBasicMaterial({ color: 0x101885 })
    );

    obj.receiveShadow = true;
    obj.castShadow = true;

    //Generate each faces
    let faceFront = generateBoard();
    let faceLeft = generateBoard();
    let faceRight = generateBoard();
    let faceBack = generateBoard();
    let faceUp = generateBoard();
    let faceDown = generateBoard();

    //From origin, space between cube obj and boards
    let space = GROUND_SIZE / 2 + BOARD_SIZE / 2

    faceFront.obj.position.z += space;
    obj.add(faceFront.obj);
    board[FRONT_F] = faceFront.cells;

    faceLeft.obj.rotation.y = Math.PI / 2;
    faceLeft.obj.position.x += space;
    obj.add(faceLeft.obj);
    board[RIGHT_F] = faceLeft.cells;

    faceRight.obj.rotation.y = -Math.PI / 2;
    faceRight.obj.position.x -= space;
    obj.add(faceRight.obj);
    board[LEFT_F] = faceRight.cells;

    faceBack.obj.rotation.y = Math.PI;
    faceBack.obj.position.z -= space
    obj.add(faceBack.obj);
    board[BACK_F] = faceBack.cells;

    faceUp.obj.rotation.x = -Math.PI / 2;
    faceUp.obj.position.y += space;
    obj.add(faceUp.obj);
    board[TOP_F] = faceUp.cells;

    faceDown.obj.rotation.x = Math.PI / 2;
    faceDown.obj.position.y -= space;
    obj.add(faceDown.obj);
    board[BOTTOM_F] = faceDown.cells;

    return { obj, board };
}

function generateBoard() {

    let obj = new THREE.Mesh();
    let cells = Array(BOARD_SIZE);

    for (let i = 0; i < BOARD_SIZE; i++) {

        cells[i] = Array(BOARD_SIZE);

        for (let j = 0; j < BOARD_SIZE; j++) {

            let cube = new THREE.Mesh(
                new THREE.BoxGeometry(BOARD_SIZE, BOARD_SIZE, BOARD_SIZE),
                new THREE.MeshBasicMaterial({ color: 0xffffff })
            );

            if (j % 2 == 0 && i % 2 == 0) {
                cube = new THREE.Mesh(
                    new THREE.BoxGeometry(BOARD_SIZE, BOARD_SIZE, BOARD_SIZE),
                    new THREE.MeshBasicMaterial({ color: 0x00ff00 })
                );
            }

            cube.receiveShadow = true;
            cube.castShadow = true;

            cells[i][j] = cube;

            let x = j;
            let y = BOARD_SIZE - 1 - i;

            x -= BOARD_SIZE / 2;
            y -= BOARD_SIZE / 2;

            cube.position.x = (BOARD_SIZE / 2) + (BOARD_SIZE) * (x);
            cube.position.y = (BOARD_SIZE / 2) + BOARD_SIZE * (y);
            obj.add(cube);
        }
    }
    return { obj, cells };
}

function setupScene() {

    let scene = new THREE.Scene();

    let camera = new THREE.PerspectiveCamera(
        FOV,
        ASPECT_RATIO
    );

    camera.position.z = 320;

    let renderer = new THREE.WebGLRenderer({ alpha: true });
    renderer.setSize(windowWidth, windowHeight);
    document.getElementById("canvas").appendChild(renderer.domElement);

    const axesHelper = new THREE.AxesHelper(1000);
    //scene.add(axesHelper);

    //Add window event listener
    // resize event listener
    window.addEventListener('resize', function () {
        resized = true
    });

    cameraControls = new THREE.OrbitControls(camera, renderer.domElement);
    cameraControls.target.set(0, 0, 0);

    return { scene, camera, renderer };
}

function resize() {

    resized = false;

    if (window.innerWidth <= window.innerHeight || ASPECT_RATIO > (window.innerWidth / window.innerHeight)) {
        windowWidth = window.innerWidth;
        windowHeight = (windowWidth) * (1 / ASPECT_RATIO);
    } else {
        windowHeight = window.innerHeight * 0.99;
        windowWidth = (windowHeight) * (ASPECT_RATIO);
    }

    setEnabledPhonemode(window.innerWidth <= window.innerHeight);

    renderer.setSize(windowWidth, windowHeight, true);

    camera.aspect = ASPECT_RATIO;
    camera.updateProjectionMatrix();
}

function refresh() {

    if (resized) { resize() }

    //Clear
    renderer.setClearColor(0);
    renderer.clear();

    //Render main camera
    renderer.setViewport(0, 0, windowWidth, windowHeight);
    renderer.render(scene, camera);

    document.getElementById("score-points").textContent = snakeModel.maxTailSize;
    snakeSpeed = initialSnakeSpeed + snakeSpeedIncrement * snakeModel.maxTailSize;
}

function animate() {

    setTimeout(() => {
        updateBoard();
        refresh();

        performingAnimation = false;

        //Update rotation X axis
        if (Math.abs(obj.rotation.x - xRotationTarget) > cubeRotationThreshold) {

            if (obj.rotation.x > xRotationTarget) {

                obj.rotation.x -= cubeRotationSpeed;

            } else {
                obj.rotation.x += cubeRotationSpeed;
            }
            performingAnimation = true;
        }

        //Update rotation Y axis
        if (Math.abs(obj.rotation.y - yRotationTarget) > cubeRotationThreshold) {

            if (obj.rotation.y >= yRotationTarget) {

                obj.rotation.y -= cubeRotationSpeed;

            } else {
                obj.rotation.y += cubeRotationSpeed;
            }

            performingAnimation = true;
        }

        requestAnimationFrame(animate);

    }, (1 / FPS) * 1000);


}

function startMove() {
    setTimeout(() => {

        snakeModel.move();

        requestAnimationFrame(startMove);

    }, (1 / Math.min(snakeSpeed, snakeMaxSpeed)));
}

function lookAtSnake() {

    let previousFace = snakeModel.previousFace;
    let currentFace = snakeModel.face;

    yRotationTarget = obj.rotation.y + transitionFacesRules[`${previousFace}`][`${currentFace}`].y;
    xRotationTarget = obj.rotation.x + transitionFacesRules[`${previousFace}`][`${currentFace}`].x;

}

function checkDeath()
{
    if (!snakeModel.alive && snakeModel.tail.length <= 0)
    {
        document.getElementById("gameover").hidden = false;
    }
}

function loadEventListeners() {

    document.addEventListener('keydown', (event) => {
        fireKey(event.key);
    });
}

function fireKey(key) {

    if (!performingAnimation) {

        switch (key) {

            case "ArrowUp":
                if (snakeModel.direction != DOWN || snakeModel.maxTailSize == 0)
                    snakeModel.direction = UP;
                break;

            case "ArrowDown":
                if (snakeModel.direction != UP || snakeModel.maxTailSize == 0)
                    snakeModel.direction = DOWN;
                break;

            case "ArrowRight":
                if (snakeModel.direction != LEFT || snakeModel.maxTailSize == 0)
                    snakeModel.direction = RIGHT;
                break;

            case "ArrowLeft":
                if (snakeModel.direction != RIGHT || snakeModel.maxTailSize == 0)
                    snakeModel.direction = LEFT;
                break;
        }
    }
}

function setEnabledPhonemode(enable) {
    document.getElementById("dpad").hidden = !enable;
}