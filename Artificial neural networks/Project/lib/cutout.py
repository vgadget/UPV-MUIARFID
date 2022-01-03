import numpy as np


"""
p : the probability that random erasing is performed
s_l, s_h : minimum / maximum proportion of erased area against input image
r_1, r_2 : minimum / maximum aspect ratio of erased area
v_l, v_h : minimum / maximum value for erased area
"""

def get_random_eraser(p=0.5, s_l=0.02, s_h=0.4, r_1=0.3, r_2=1/0.3, v_l=0, v_h=255):
    
    #DEFINE DATA AUGMENTATION CALLBACK 
    def eraser(input_img):
        
        img_h, img_w, img_c = input_img.shape

        if p < np.random.rand():
            return input_img


        erasing = True
        while erasing:
            s = np.random.uniform(s_l, s_h) * img_h * img_w
            r = np.random.uniform(r_1, r_2)
            w = int(np.sqrt(s / r))
            h = int(np.sqrt(s * r))
            left = np.random.randint(0, img_w)
            top = np.random.randint(0, img_h)

            erasing = not (left + w <= img_w and top + h <= img_h)

        input_img[top:top + h, left:left + w] = np.random.uniform(v_l, v_h, (h, w, img_c))

        return input_img

    return eraser
