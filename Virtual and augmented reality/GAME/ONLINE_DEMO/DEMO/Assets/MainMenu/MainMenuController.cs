using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using UnityEngine.SceneManagement;

public class MainMenuController : MonoBehaviour
{
    public TMP_InputField usernameTextField;

    void Start()
    {
        
    }

    void Update()
    {
        
    }

    public void OnClick()
    {
        OnlineObject.username = usernameTextField.text;
        SceneManager.LoadScene("World", LoadSceneMode.Single);
    }

}
