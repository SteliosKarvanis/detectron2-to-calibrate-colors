from detectron2 import model_zoo
from detectron2.data import DatasetCatalog, MetadataCatalog, build_detection_test_loader
from detectron2.evaluation import COCOEvaluator, inference_on_dataset
from detectron2.engine import DefaultPredictor
from detectron2.utils.logger import setup_logger
from detectron2.structures import BoxMode
from detectron2.evaluation import DatasetEvaluator
import detectron2.data.transforms as T
from detectron2.config import get_cfg
import matplotlib.pyplot as plt
import numpy as np
import os
import json

from utils import *
from constants import *


class Counter(DatasetEvaluator):
  def reset(self):
    self.count = 0
    self.inps = [0] * len(classes)
    self.outps = [0] * len(classes)
  def process(self, inputs, outputs):
    for input in inputs["annotations"]:
      self.inps[int(input["category_id"])] += 1
    for output in outputs:
      for i in range(len(output["instances"])):
        self.outps[int(output["instances"][i].pred_classes)] += 1

  def evaluate(self):
    out = {}
    for index, item in enumerate(classes):
      if self.inps[index] != 0:
        out[item] = self.outps[index]/self.inps[index]
    return out

def get_all_inputs_outputs(dataset):
  inp_data = get_data(dataset_dir + dataset)
  for index, data in enumerate(build_detection_test_loader(cfg, dataset)):
      yield inp_data[index], predictor.model(data)



for d in ["train", "test"]:
    DatasetCatalog.register(d, lambda d=d: get_data(dataset_dir + d))
    MetadataCatalog.get(d).set(thing_classes=classes)

cfg = custom_cfg()

cfg.MODEL.WEIGHTS = os.path.join(cfg.OUTPUT_DIR, "model_final.pth") 
cfg.MODEL.ROI_HEADS.SCORE_THRESH_TEST = 0.5
cfg.MODEL.DEVICE='cpu'

predictor = DefaultPredictor(cfg)


evaluator = Counter()
evaluator.reset()
for inputs, outputs in get_all_inputs_outputs("test"):
  evaluator.process(inputs, outputs)
eval_results = evaluator.evaluate()

