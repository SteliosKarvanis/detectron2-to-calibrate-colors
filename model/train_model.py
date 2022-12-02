from constants import *
from utils import *

setup_logger()
#manage_datasets()
load_datasets()
cfg = custom_cfg()
os.makedirs(cfg.OUTPUT_DIR, exist_ok=True)
trainer = DefaultTrainer(cfg)
trainer.resume_or_load(resume=False)
trainer.train()
