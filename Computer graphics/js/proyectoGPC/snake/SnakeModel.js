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


//PARTICLES
const EMPTY = 0;
const PLAYER = 1;
const TAIL = 2;
const FOOD = 3;
const MAX_GENERATED_FOOD = 3;

//MOVEMENT
const UP = 0;
const DOWN = 1;
const RIGHT = 2;
const LEFT = 3;

//FACES
const FRONT_F = 0;
const BACK_F = 1;
const RIGHT_F = 2;
const LEFT_F = 3;
const TOP_F = 4;
const BOTTOM_F = 5;

class SnakeModel {

    constructor(size, observers = []) {

        //Current coordinates
        this.face = FRONT_F;
        this.previousFace = FRONT_F;
        this.x = parseInt(size / 2);
        this.y = parseInt(size / 2);

        // Current direction
        this.direction = RIGHT;

        //Tail size starts 0
        this.maxTailSize = 0;
        this.tail = [];
        this.remainingFood = 0;

        //Generate cube model with given face size 3D matrix [FACE][X][Y]
        this.maze = Array(6);
        for (let i = 0; i < this.maze.length; i++) {
            this.maze[i] = Array(size);

            for (let j = 0; j < size; j++) {
                this.maze[i][j] = Array(size);

                for (let k = 0; k < size; k++) {
                    this.maze[i][j][k] = EMPTY;
                }
            }
        }

        this.mazeSize = size;
        this.maze[this.face][this.x][this.y] = PLAYER;
        this.alive = true;
        this.observers = observers;
    }


    move() {

        if (this.alive) {

            //Remove player from scene
            this.maze[this.face][this.x][this.y] = EMPTY;

            let desiredX = this.x;
            let desiredY = this.y;

            switch (this.direction) {

                case UP:
                    desiredX--;
                    break;
                case DOWN:
                    desiredX++;
                    break;
                case RIGHT:
                    desiredY++;
                    break;
                case LEFT:
                    desiredY--;
                    break;
            }

            let faceTransitionDirection = false;

            //Check if desired movement is legal in X
            if (desiredX >= this.mazeSize) {
                desiredX = 0;
                faceTransitionDirection = DOWN;
            } else if (desiredX < 0) {
                desiredX = this.mazeSize - 1;
                faceTransitionDirection = UP;
            }

            //Check if desired movement is legal in Y
            if (desiredY >= this.mazeSize) {
                desiredY = 0;
                faceTransitionDirection = RIGHT;
            } else if (desiredY < 0) {
                desiredY = this.mazeSize - 1;
                faceTransitionDirection = LEFT;
            }

            let previousX = this.x;
            let previousY = this.y;
            let previousFace = this.face;
            this.x = desiredX;
            this.y = desiredY;

            if (faceTransitionDirection !== false) {
                let nextFace = this.getNextFace(faceTransitionDirection);
                this.adaptCoordinates(nextFace)
                this.face = nextFace;
                this.previousFace = previousFace;
                this.updateObservers();
            }

            if (this.maze[this.face][this.x][this.y] == TAIL) {
                this.alive = false;
                this.maze[previousFace][previousX][previousY] = PLAYER;
            }
            else {

                if (this.maze[this.face][this.x][this.y] == FOOD) {
                    this.remainingFood--;
                    this.maxTailSize++;
                }

                if (this.remainingFood <= 0) {
                    this.spawnFood();
                }

                this.maze[this.face][this.x][this.y] = PLAYER;
            }

            //Remove last tail
            if (this.tail.length >= this.maxTailSize && this.maxTailSize > 0) {
                let lastTail = this.tail.pop();
                let tf, tx, ty;
                tf = lastTail[0];
                tx = lastTail[1];
                ty = lastTail[2];
                this.maze[tf][tx][ty] = EMPTY;
            }

            // Add tail
            if (this.tail.length < this.maxTailSize) {
                this.maze[previousFace][previousX][previousY] = TAIL;
                this.tail.unshift([previousFace, previousX, previousY]);
            }
        } else {
            //Remove last tail
            if (this.tail.length > 0) {
                let lastTail = this.tail.pop();
                let tf, tx, ty;
                tf = lastTail[0];
                tx = lastTail[1];
                ty = lastTail[2];
                this.maze[tf][tx][ty] = EMPTY;
            } else 
            {
                this.updateObservers();
            }
        }
    }


    adaptCoordinates(desiredFace) {

        // Addapt coordinates if top face or bottom face are involved in

        //To top face
        if (desiredFace == TOP_F) {

            switch (this.face) {

                case RIGHT_F:
                    [this.x, this.y] = [this.mazeSize - 1 - this.y, this.x];
                    this.direction = LEFT;
                    break;
                case LEFT_F:
                    [this.x, this.y] = [this.y, this.mazeSize - 1 - this.x];
                    this.direction = RIGHT;
                    break;
                case BACK_F:
                    [this.x, this.y] = [this.mazeSize - 1 - this.x, this.mazeSize - 1 - this.y];
                    this.direction = DOWN;
                    break;
            }
        }
        // From Top face 
        else if (this.face == TOP_F) {

            switch (desiredFace) {

                case RIGHT_F:
                    [this.x, this.y] = [this.y, this.mazeSize - 1 - this.x];
                    this.direction = DOWN;
                    break;
                case LEFT_F:
                    [this.x, this.y] = [this.mazeSize - 1 - this.y, this.x];
                    this.direction = DOWN;
                    break;
                case BACK_F:
                    [this.x, this.y] = [this.mazeSize - 1 - this.x, this.mazeSize - 1 - this.y];
                    this.direction = DOWN;
                    break;
            }
        }
        //To bottom face
        else if (desiredFace == BOTTOM_F) {

            switch (this.face) {

                case RIGHT_F:
                    [this.x, this.y] = [this.y, this.mazeSize - 1 - this.x];
                    this.direction = LEFT;
                    break;
                case LEFT_F:
                    [this.x, this.y] = [this.mazeSize - 1 - this.y, this.x];
                    this.direction = RIGHT;
                    break;
                case BACK_F:
                    [this.x, this.y] = [this.mazeSize - 1 - this.x, this.mazeSize - 1 - this.y];
                    this.direction = UP;
                    break;
            }
        }
        // From bottom face
        else if (this.face == BOTTOM_F) {

            switch (desiredFace) {

                case RIGHT_F:
                    [this.x, this.y] = [this.mazeSize - 1 - this.y, this.x];
                    this.direction = UP;
                    break;
                case LEFT_F:
                    [this.x, this.y] = [this.y, this.mazeSize - 1 - this.x];
                    this.direction = UP;
                    break;
                case BACK_F:
                    [this.x, this.y] = [this.mazeSize - 1 - this.x, this.mazeSize - 1 - this.y];
                    this.direction = UP;
                    break;
            }
        }




    }

    getNextFace(movement) {

        let nextFace = this.face;

        switch (this.face) {

            case FRONT_F:

                switch (movement) {

                    case UP:
                        nextFace = TOP_F;
                        break;
                    case DOWN:
                        nextFace = BOTTOM_F;
                        break;
                    case LEFT:
                        nextFace = LEFT_F;
                        break;
                    case RIGHT:
                        nextFace = RIGHT_F;
                        break;
                }

                break;

            case RIGHT_F:

                switch (movement) {

                    case UP:
                        nextFace = TOP_F;
                        break;
                    case DOWN:
                        nextFace = BOTTOM_F;
                        break;
                    case LEFT:
                        nextFace = FRONT_F;
                        break;
                    case RIGHT:
                        nextFace = BACK_F;
                        break;
                }

                break;

            case LEFT_F:

                switch (movement) {

                    case UP:
                        nextFace = TOP_F;
                        break;
                    case DOWN:
                        nextFace = BOTTOM_F;
                        break;
                    case LEFT:
                        nextFace = BACK_F;
                        break;
                    case RIGHT:
                        nextFace = FRONT_F;
                        break;
                }

                break;

            case BACK_F:

                switch (movement) {

                    case UP:
                        nextFace = TOP_F;
                        break;
                    case DOWN:
                        nextFace = BOTTOM_F;
                        break;
                    case LEFT:
                        nextFace = RIGHT_F;
                        break;
                    case RIGHT:
                        nextFace = LEFT_F;
                        break;
                }

                break;

            case TOP_F:

                switch (movement) {

                    case UP:
                        nextFace = BACK_F;
                        break;
                    case DOWN:
                        nextFace = FRONT_F;
                        break;
                    case LEFT:
                        nextFace = LEFT_F;
                        break;
                    case RIGHT:
                        nextFace = RIGHT_F;
                        break;
                }

                break;

            case BOTTOM_F:

                switch (movement) {

                    case UP:
                        nextFace = FRONT_F;
                        break;
                    case DOWN:
                        nextFace = BACK_F;
                        break;
                    case LEFT:
                        nextFace = LEFT_F;
                        break;
                    case RIGHT:
                        nextFace = RIGHT_F;
                        break;
                }

                break;
        }

        return nextFace;
    }

    updateObservers() {
        this.observers.forEach(fn => {
            fn();
        });
    }

    spawnFood() {

        for (let k = 0; k < this.maze.length; k++) {

            const face = this.maze[k];

            let i = 0, j = 0;

            while (i == j || face[i][j] != EMPTY) {
                i = Math.floor(Math.random() * face.length);
                j = Math.floor(Math.random() * face[i].length);
            }
            face[i][j] = FOOD;
            this.remainingFood++;
        }

    }
}