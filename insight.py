import os
import argparse
import cv2
import sys
import numpy as np
import insightface
from insightface.app import FaceAnalysis 

assert insightface.__version__>='0.3'

parser = argparse.ArgumentParser(description='insightface app test')
# general
parser.add_argument('--ctx', default=0, type=int, help='ctx id, <0 means using cpu')
parser.add_argument('--det-size', default=640, type=int, help='detection size')
args = parser.parse_args()

app = FaceAnalysis(name='antelopev2')
app.prepare(ctx_id=args.ctx, det_size=(args.det_size,args.det_size))

name_list=[]
cnt=0
for f in os.listdir('./feret'):
    if f[-3:]=='jpg':
        name_list.append(f)

fout=open('vec_insight_feret','w')

for name in name_list:
    img = cv2.imread('./feret/'+name)
    faces = app.get(img)
    if len(faces)!=1:
        continue
    
    img_embedding = faces[0].normed_embedding
    fout.write(name+' ')
    for x in img_embedding:
        fout.write(str(float(x))+' ')
    fout.write('\n')
    cnt+=1
    print(cnt,len(name_list))