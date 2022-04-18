/*

 ,----.   ,------.  ,-----.               ,------.  ,--. 
'  .-./   |  .--. ''  .--./    ,-----.    |  .--. '/   | 
|  | .---.|  '--' ||  |        '-----'    |  '--' |`|  | 
'  '--'  ||  | --' '  '--'\               |  | --'  |  | 
 `------' `--'      `-----'               `--'      `--'

2021/2022
(c) ADRIÁN VÁZQUEZ BARRERA

                                                    
*/

//Shader de vertices
var VERTEX_SHADER_SOURCE = `

    attribute vec3 posicion;
    varying highp vec3 color;

    void main()
    {
        float distance = sqrt((posicion[0]*posicion[0]) + (posicion[1]*posicion[1]));
        distance = distance/2.0;
        gl_Position = vec4(posicion, 1.0);
        gl_PointSize = 10.0;
        color = vec3(1.0-distance , 1.0-distance, 1.0-distance);
    }
`;

//Shader de fragmentos
var FRAGMENT_SHADER_SOURCE = `

    varying highp vec3 color;

    void main()
    {
        gl_FragColor = vec4(color, 1.0);
    }
`;


function main()
{

    let points = [
        // X, Y, Z
        0, 0, 0,
        ];

    let {canvas, gl, program} = setupCanvas();

    let {vertexShader, fragmentShader} = setupShaders(gl, program)

    //Enlazar el programa al WebGL
    linkProgram(gl, program);

    //Validar programa
    validateProgram(gl, program);

    //Crear buffers
    let {posicion_shad_attrib_loc, pointsBufferObj} = setupBuffers(gl, program, points);

    gl.useProgram(program);

    draw(gl, points);

    canvas.onmousedown = event => {clickEvent(event, gl, canvas, points)}
    
}

function setupCanvas()
{
    let canvas = document.getElementById('canvas');
    let gl = canvas.getContext('webgl');
    let program = gl.createProgram();

    if (!gl)
    {
        alert("Tu navegador no soporta WebGL");
        return false;
    }

    gl.clearColor(0.0, 0.0, 0.85, 1.0);
    clearCanvas(gl);

    return {canvas, gl, program};
}

function clearCanvas(gl)
{
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
}

function setupShaders(gl, program)
{

    //Configuración shaders
    let vertexShader = gl.createShader(gl.VERTEX_SHADER);
    let fragmentShader = gl.createShader(gl.FRAGMENT_SHADER)
    gl.shaderSource(vertexShader, VERTEX_SHADER_SOURCE);
    gl.shaderSource(fragmentShader, FRAGMENT_SHADER_SOURCE);
    gl.compileShader(vertexShader);
    gl.compileShader(fragmentShader);

    //Comprobar errores de compilacion de los shaders

    if (!gl.getShaderParameter(vertexShader, gl.COMPILE_STATUS) || !gl.getShaderParameter(fragmentShader, gl.COMPILE_STATUS) )
    {
        alert("Error de compilación en los shader");
        alert(gl.getShaderInfoLog(vertexShader));
        alert(gl.getShaderInfoLog(fragmentShader));
        return false;
    }

    //Añadir los shader al programa
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);

    return {vertexShader, fragmentShader};
}

function linkProgram(gl, program)
{
    gl.linkProgram(program);
    if (!gl.getProgramParameter(program, gl.LINK_STATUS))
    {
        alert("Error en el enlazado del prograna");
        return false;
    }

    return true;
}

function validateProgram(gl, program)
{
    gl.validateProgram(program);
    if (!gl.getProgramParameter(program, gl.VALIDATE_STATUS))
    {
        alert("Error validar el programa");
        return false;
    }

    return true;
}

function setupBuffers(gl, program, points)
{
    let pointsBufferObj = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, pointsBufferObj);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(points), gl.STATIC_DRAW);

    //Atributo del shader llamado 'posicion'
    let posicion_shad_attrib_loc = gl.getAttribLocation(program, 'posicion');

    gl.vertexAttribPointer(
        posicion_shad_attrib_loc,
        3, //Tres elementos del array por atributo
        gl.FLOAT, //Tipo de dato de los elementos del array
        gl.FALSE, //Los datos no estan normalizados
        3 * Float32Array.BYTES_PER_ELEMENT, //Tamaño por cada vertice, cada vertice tiene 3 dimensiones por tam de un float
        0 //Offset
    );

    gl.enableVertexAttribArray(posicion_shad_attrib_loc);

    return {posicion_shad_attrib_loc, pointsBufferObj}
}

function draw(gl, points)
{
    clearCanvas(gl);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(points), gl.STATIC_DRAW);
    gl.drawArrays(gl.LINE_STRIP, 0, points.length/3);
    gl.drawArrays(gl.POINTS, 0, points.length/3);
}

function clickEvent(event, gl, canvas, points)
{
    //Posicion del mouse respecto del body
    let x = event.clientX;
    let y = event.clientY;

    //Rectangulo del canvas
    let rectangle = event.target.getBoundingClientRect(); 

    //Conversion de puntos respecto del canvas
    x = (( (x - rectangle.left) - (canvas.width/2) ) * 2)/canvas.width;
    y = ((canvas.height/2 - (y - rectangle.top)) * 2)/canvas.height;
    z = 0;

    points.push(x);
    points.push(y);
    points.push(z);

    draw(gl, points);
}