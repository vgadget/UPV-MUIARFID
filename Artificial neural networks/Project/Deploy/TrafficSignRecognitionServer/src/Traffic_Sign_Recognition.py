import cv2
import numpy as np
import tensorflow as tf
from tensorflow.keras.optimizers import Adam
from tensorflow.keras import models

model_name = "./TSR_Model_98acc50Epoch_MACM1"
img_size = 32
epochs = 50
learning_rate = 0.001
batch_size = 1024

model = models.load_model(model_name)

optimizer = Adam(
    learning_rate = learning_rate, 
    decay = (learning_rate / epochs)
)

model.compile(
    optimizer = optimizer, 
    loss = "categorical_crossentropy", 
    metrics = ["accuracy"]
)


## PREDICT
def predictClass(img_str):
    
    nparr = np.fromstring(img_str, np.uint8)
    
    image = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
    
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    
    image_rs = cv2.resize(image, (img_size, img_size), 3)     

    R, G, B = cv2.split(image_rs)     

    img_r = cv2.equalizeHist(R)
    img_g = cv2.equalizeHist(G)
    img_b = cv2.equalizeHist(B)        

    image = cv2.merge((img_r, img_g, img_b))
    
    #cv2.imshow("window_name", image)  
    #cv2.waitKey(0)  
    
    input_arr = tf.keras.preprocessing.image.img_to_array(image)
    input_arr = np.array([input_arr])  # Convert single image to a batch.
    input_arr = input_arr.astype('float32') / 255. 
    predictions = model.predict(input_arr)
    predicted_class = np.argmax(predictions, axis=-1)[0]
    
    return (predicted_class,  image)