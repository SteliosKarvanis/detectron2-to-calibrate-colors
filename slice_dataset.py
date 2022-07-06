import numpy as np
import shutil
import os

dataset_dir = "dataset/"
allFileNames = np.arange(50)
np.random.shuffle(allFileNames)
train_ratio = 0.8
is_new = False

if not os.path.isdir(dataset_dir +'train/'):
    os.makedirs(dataset_dir +'train/')
    is_new = True
if not os.path.isdir(dataset_dir +'val/'):
    os.makedirs(dataset_dir +'val/')
    is_new = True

if is_new:
    train_FileNames, val_FileNames = np.split(np.array(allFileNames), [int(len(allFileNames)* train_ratio)])
    for name in train_FileNames:
        shutil.copy(dataset_dir+"test_"+str(name)+".jpg", "dataset/train")
        shutil.copy(dataset_dir+"test_"+str(name)+".json", "dataset/train")

    for name in val_FileNames:
        shutil.copy(dataset_dir+"test_"+str(name)+".jpg", "dataset/val")
        shutil.copy(dataset_dir+"test_"+str(name)+".json", "dataset/val")