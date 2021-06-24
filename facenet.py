from facenet_pytorch import MTCNN, InceptionResnetV1
from PIL import Image
import numpy as np
import os
# If required, create a face detection pipeline using MTCNN:
mtcnn = MTCNN()

# Create an inception resnet (in eval mode):
resnet = InceptionResnetV1(pretrained='vggface2').eval()



name_list=[]
 

for f in os.listdir('./images'):
    name_list.append(f)

vec=[]
f=open('vec3','w')
for name in name_list:
    img = Image.open('./images/'+name)
    img_cropped = mtcnn(img)
    img_embedding = resnet(img_cropped.unsqueeze(0))
    vec.append(img_embedding)
    f.write(name)
    for x in img_embedding[0]:
        f.write(' '+str(float(x)))
    f.write('\n')


