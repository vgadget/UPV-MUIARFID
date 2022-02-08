using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using WebSocketSharp;

public class WebsocketClient : MonoBehaviour
{
    public string SOCKET_URL_SERVER = "ws://localhost:8080"; 

    private static WebSocket ws;
    private static List<string> data = new List<string>();

    bool debug = true;

    void Start()
    {
        if (ws == null)
        {
            ws = new WebSocket(SOCKET_URL_SERVER);
            ws.Connect();
            ws.OnMessage += (sender, e) =>
            {
                data.Add(e.Data);

                if (debug)
                {
                    Debug.Log(e.Data);
                }

            };
            
        }

    }

    public static void SendData(string data)
    {
        if (ws != null)
            ws.Send(data);
    }

    public static List<string> GetData()
    {
        return data;
    }

}
