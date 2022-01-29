using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using WebSocketSharp;

public class WebsocketClient : MonoBehaviour
{
    public string SOCKET_URL_SERVER = "ws://localhost:8080"; 

    private WebSocket ws;

    void Start()
    {
        ws = new WebSocket(SOCKET_URL_SERVER);
        ws.Connect();
        ws.OnMessage += (sender, e) =>
        {
            Debug.Log("Data : " + e.Data);
        };
        ws.Send("Hello from unity");

    }

    // Update is called once per frame
    void Update()
    {

    }
}
