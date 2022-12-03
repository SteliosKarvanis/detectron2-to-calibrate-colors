from constants import *
from utils import *
from detectron2.data import build_detection_test_loader, build_detection_train_loader

setup_logger()
#manage_datasets()
load_datasets()
cfg = custom_cfg()
os.makedirs(cfg.OUTPUT_DIR, exist_ok=True)

class CustomTrainer(DefaultTrainer):
    @classmethod
    def build_train_loader(cls, cfg):
        return build_detection_train_loader(cfg, mapper=custom_mapper)

trainer = DefaultTrainer(cfg)
trainer.resume_or_load(resume=False)
trainer.train()
