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


//Video options
const FPS = 120;
const FOV = 75;
const ASPECT_RATIO = 16 / 10;
const initialSnakeSpeed = 0.008;
const snakeSpeedIncrement = 0.0005;
const snakeMaxSpeed = 0.012;
const cubeRotationSpeed = Math.PI * 0.025;
const cubeRotationThreshold = 0.01;

const MAIN_COLOR = 0x00ff00;
const SECONDARY_COLOR = 0x6ba36a;
const TERTIARY_COLOR = 0x6bf36a;

const transitionFacesRules = {

    //From front face
    0: {
        //To back face
        1: {
            x: 0,
            y: -Math.PI,
        },

        //To right face
        2: {
            x: 0,
            y: -Math.PI / 2,
        },

        //To left face
        3: {
            x: 0,
            y: Math.PI / 2,
        },

        //To top face
        4: {
            x: Math.PI / 2,
            y: 0,
        },

        //To bottom face
        5: {
            x: -Math.PI / 2,
            y: 0,
        },
    },

    //From back face
    1: {
        //To front face
        0: {
            x: 0,
            y: -Math.PI,
        },

        //To right face
        2: {
            x: 0,
            y: Math.PI / 2,
        },

        //To left face
        3: {
            x: 0,
            y: -Math.PI / 2,
        },

        //To top face
        4: {
            x: Math.PI/2,
            y: -Math.PI,
        },

        //To bottom face
        5: {
            x: -Math.PI/2,
            y: Math.PI,
        },
    },

    //From right face
    2: {
        //To back face
        1: {
            x: 0,
            y: -Math.PI / 2,
        },

        //To front face
        0: {
            x: 0,
            y: Math.PI / 2,
        },

        //To left face
        3: {
            x: 0,
            y: -Math.PI,
        },

        //To top face
        4: {
            x: Math.PI / 2,
            y: Math.PI / 2,
        },

        //To bottom face
        5: {
            x: -Math.PI / 2,
            y: Math.PI / 2,
        },
    },

    //From left face
    3: {
        //To back face
        1: {
            x: 0,
            y: Math.PI / 2,
        },

        //To right face
        2: {
            x: 0,
            y: Math.PI,
        },

        //To front face
        0: {
            x: 0,
            y: -Math.PI / 2,
        },

        //To top face
        4: {
            x: Math.PI/2,
            y: -Math.PI/2,
        },

        //To bottom face
        5: {
            x: -Math.PI/2,
            y: -Math.PI/2,
        },
    },

    //From top face
    4: {
        //To back face
        1: {
            x: -Math.PI/2,
            y: -Math.PI,
        },

        //To right face
        2: {
            x: -Math.PI/2,
            y: -Math.PI/2,
        },

        //To left face
        3: {
            x: -Math.PI/2,
            y: Math.PI/2,
        },

        //To front face
        0: {
            x: -Math.PI/2,
            y: 0,
        },
    },

    //From bottom face
    5: {
        //To back face
        1: {
            x: Math.PI / 2,
            y: -Math.PI,
            z: 0
        },

        //To right face
        2: {
            x: Math.PI / 2,
            y: -Math.PI/2,
        },

        //To left face
        3: {
            x: Math.PI / 2,
            y: Math.PI/2,
        },

        //To front face
        0: {
            x: Math.PI / 2,
            y: 0,
        },
    }
};

//Game options
const BOARD_SIZE = 15;

