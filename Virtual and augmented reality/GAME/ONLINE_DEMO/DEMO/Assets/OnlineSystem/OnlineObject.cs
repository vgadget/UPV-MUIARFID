using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.Threading.Tasks;
using UnityEngine;

public class OnlineObject : MonoBehaviour
{

    public static string username = "player 1";
    public static int priority = 0;

    public int refreshRate = 5000;

    private static readonly Dictionary<string, GameObject> onlineObject = new Dictionary<string, GameObject>();
    private double posx, posy, posz;
    private double speedx, speedy, speedz;

    private Rigidbody rb;

    private List<Vector3> positions =new List<Vector3>();
    private List<string> users = new List<string>();
    System.Random rnd;

    async void Start()
    {
        rnd = new System.Random();
        rb = GetComponent<Rigidbody>();

        //UpdateStatus();
        //19115;Luis;2.95713424682617;0.121429949998856;-0.751287817955017;0;0;0
        //22306;Luis;-3.12904047966003;0.141943946480751;-1.10325133800507;0;0;0
        // 25755;Luis;-0.771504998207092;0.128989323973656;1.22862839698792;0;0;0
        // 27982;Luis;0.814083158969879;0.121399037539959;-7.08038663864136;0;0;0

        positions.Add(new Vector3(2.957f, 0.1214f, -0.751f));
        positions.Add(new Vector3(-3.129f, 0.141f, -1.10325f));
        positions.Add(new Vector3(-0.7715f, 0.129f, 1.2286f));
        positions.Add(new Vector3(0.814f, 0.121399f, -7.0803866f));

        users.Add("German");
        users.Add("Adri");
        users.Add("EmeJota");
        users.Add("Mariano");
        users.Add("Xokas");
        users.Add("Nathalie");
        users.Add("Lorenzo");
        users.Add("Jorge");
        users.Add("Ferran");
        users.Add("M. Carmen");

    }


    void Update()
    {
            Vector3 randomPos1, randomPos2, randomPosMax, randomPosMin;
            string randomUser;

            randomPos1 = positions[rnd.Next(0, positions.Count)];
            randomPos2 = positions[rnd.Next(0, positions.Count)];
            randomUser = users[rnd.Next(0, positions.Count)];

            randomPosMax = new Vector3(
                Math.Max(randomPos1.x, randomPos2.x),
                Math.Max(randomPos1.y, randomPos2.y),
                Math.Max(randomPos1.z, randomPos2.z)
             );

            randomPosMin = new Vector3(
                Math.Min(randomPos1.x, randomPos2.x),
                Math.Min(randomPos1.y, randomPos2.y),
                Math.Min(randomPos1.z, randomPos2.z)
             );

            posx = (rnd.NextDouble() * (randomPosMax.x - randomPosMin.x) + randomPosMin.x);
            posy = (rnd.NextDouble() * (randomPosMax.y - randomPosMin.y) + randomPosMin.y);
            posz = (rnd.NextDouble() * (randomPosMax.z - randomPosMin.z) + randomPosMin.z);
            speedx = 0f;
            speedy = 0f;
            speedz = 0f;

            username = users[rnd.Next(0, users.Count)];
            WebsocketClient.SendData(EncondeData());
        

    }


    async Task<Task> UpdateStatus()
    {
        while (true)
        {
            await Task.Run(() =>
            {
                WebsocketClient.SendData(EncondeData());
                DecodeData(WebsocketClient.GetData());
                WebsocketClient.GetData().Clear();
            });

            await Task.Delay(refreshRate);
        }

        return Task.Delay(refreshRate);

    }

    static void DecodeData(List<string> data)
    {

        float px, py, pz;
        float sx, sy, sz;
        string n;
        int prior;

        foreach (string d in data)
        {
            string[] s = d.Split(';');

            prior = StringToInteger(s[0]);
            n = s[1];
            px = StringToFloat(s[2]);
            py = StringToFloat(s[3]);
            pz = StringToFloat(s[4]);
            sx = StringToFloat(s[5]);
            sy = StringToFloat(s[6]);
            sz = StringToFloat(s[7]);
            
            // If friend already registered
            if (onlineObject.ContainsKey(n))
            {
                onlineObject[n].transform.position = new Vector3(px, py, pz);
                onlineObject[n].GetComponent<Rigidbody>().velocity = new Vector3(sx, sy, sz);
            } else
            {

            }

            Debug.Log(d);
        }
    }


    string EncondeData()
    {
        return NumberToString(priority++) + ";" + username + ";"
            + NumberToString(posx) + ";" + NumberToString(posy) + ";" + NumberToString(posz) + ";"
            + NumberToString(speedx) + ";" + NumberToString(speedy) + ";" + NumberToString(speedz);
    }

    public static string NumberToString(int n)
    {
        return n.ToString(CultureInfo.InvariantCulture);
    }

    public static string NumberToString(double n)
    {
        return n.ToString(CultureInfo.InvariantCulture);
    }

    public static int StringToInteger(string s)
    {
        int res = 0;
        int.TryParse(s, NumberStyles.Integer, CultureInfo.InvariantCulture, out res);
        return res;
    }

    public static float StringToFloat(string s)
    {
        float res = 0f;
        float.TryParse(s, NumberStyles.Float, CultureInfo.InvariantCulture, out res);
        return res;
    }
}
