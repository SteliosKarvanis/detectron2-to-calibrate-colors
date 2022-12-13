import cv2
from detectron2 import model_zoo
from detectron2.data import DatasetCatalog, MetadataCatalog, DatasetMapper, build_detection_test_loader, build_detection_train_loader
from detectron2.evaluation import COCOEvaluator, inference_on_dataset
from detectron2.engine import DefaultTrainer, DefaultPredictor
from detectron2.utils.visualizer import Visualizer, ColorMode
from detectron2.data import detection_utils as utils
from detectron2.utils.logger import setup_logger
from detectron2.structures import BoxMode
import detectron2.data.transforms as T
from detectron2.config import get_cfg
import matplotlib.pyplot as plt
import numpy as np
import os
import json
import random
import shutil
import copy
import torch
import glob
from constants import *
from utils import *


def manage_datasets():
    os.makedirs(train_dir, exist_ok=True)
    os.makedirs(test_dir, exist_ok=True)
    all_fileNames = glob.glob(dataset_dir+"*.jpg")
    for file in [file.removesuffix('.jpg') for file in all_fileNames]:
        index = file.split('_')[1]
        new_name = "new_" + index
        os.rename(file + str(".jpg"), dataset_dir + new_name + str(".jpg"))
        os.rename(file + str(".json"), dataset_dir + new_name + str(".json"))
    np.random.shuffle(all_fileNames)
    train_filenames, test_filenames = np.split(np.array(all_fileNames), [int(len(all_fileNames)* train_ratio)])
    for filename in [name[:name.find('.')] for name in train_filenames]:
        shutil.move(str(filename) + '.json', train_dir)
        shutil.move(str(filename) + '.jpg', train_dir)
    for filename in [name[:name.find('.')] for name in test_filenames]:
        shutil.move(str(filename) + '.json', test_dir)
        shutil.move(str(filename) + '.jpg', test_dir)


def get_data(img_dir):
    dataset_dicts = []
    for index, filename in enumerate([file for file in os.listdir(img_dir) if file.endswith('.json')]):
        json_file = os.path.join(img_dir, filename)
        with open(json_file) as f:
            img_label = json.load(f)

        record = {}

        img = os.path.join(img_dir, filename.removesuffix('.json') + str('.jpg'))
        
        record["file_name"] = img
        record["height"] = camera_rows
        record["width"] = camera_cols
        record["image_id"] = index
        shapes = img_label["shapes"]
        objs = []
        for shape in shapes:
            px = [a[0] for a in shape['points']]
            py = [a[1] for a in shape['points']]
            poly = [(x, y) for x, y in zip(px, py)]
            poly = [p for x in poly for p in x]

            obj = {
                "bbox": [np.min(px), np.min(py), np.max(px), np.max(py)],
                "bbox_mode": BoxMode.XYXY_ABS,
                "segmentation": [poly],
                "category_id": classes.index(shape['label']),
                "iscrowd": 0
            }
            objs.append(obj)
        record["annotations"] = objs
        dataset_dicts.append(record)
    return dataset_dicts


def load_datasets():
    for d in ["train", "test"]:
        DatasetCatalog.register(d, lambda d=d: get_data(dataset_dir + d))
        MetadataCatalog.get(d).set(thing_classes=classes)


def custom_cfg():
    cfg = get_cfg()
    cfg.merge_from_file(model_zoo.get_config_file("COCO-InstanceSegmentation/mask_rcnn_R_50_FPN_3x.yaml"))
    cfg.MODEL.WEIGHTS = model_zoo.get_checkpoint_url("COCO-InstanceSegmentation/mask_rcnn_R_50_FPN_3x.yaml")
    cfg.MODEL.DEVICE = "cuda"
    cfg.DATASETS.TRAIN = ("train",)
    cfg.DATASETS.TEST = ("test",)
    cfg.DATALOADER.NUM_WORKERS = 2
    cfg.SOLVER.IMS_PER_BATCH = 3
    cfg.INPUT.RANDOM_FLIP = "horizontal"
    cfg.SOLVER.BASE_LR = 0.0025
    cfg.SOLVER.MAX_ITER = 300
    cfg.MODEL.ROI_HEADS.SCORE_THRESH_TEST = 0.5
    cfg.SOLVER.STEPS = []   
    cfg.MODEL.ROI_HEADS.BATCH_SIZE_PER_IMAGE = 512 
    cfg.MODEL.ROI_HEADS.NUM_CLASSES = len(classes) 
    return cfg


def custom_mapper(dataset_dict):
    dataset_dict = copy.deepcopy(dataset_dict)
    image = utils.read_image(dataset_dict["file_name"], format="RGB")
    transform_list = [T.Resize((800,800)),
                      T.RandomFlip(prob=0.5, horizontal=False, vertical=True),
                      T.RandomFlip(prob=0.5, horizontal=True, vertical=False), 
                    #T.RandomContrast(0.8, 1.3),
                    #T.RandomSaturation(0.8, 1.3),
                    T.RandomRotation(angle=[90, 90]),
                          T.RandomBrightness(0.1, 2),
                            T.RandomCrop("absolute", (640, 640))
                      ]
    image, transforms = T.apply_transform_gens(transform_list, image)
    dataset_dict["image"] = torch.as_tensor(image.transpose(2, 0, 1).astype("float32"))

    annos = [
        utils.transform_instance_annotations(obj, transforms, image.shape[:2])
        for obj in dataset_dict.pop("annotations")
        if obj.get("iscrowd", 0) == 0
    ]
    instances = utils.annotations_to_instances(annos, image.shape[:2])
    dataset_dict["instances"] = utils.filter_empty_instances(instances)
    return dataset_dict