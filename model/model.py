from constants import *
from utils import *


cfg = custom_cfg()
cfg.MODEL.WEIGHTS = os.path.join(cfg.OUTPUT_DIR, "model_final.pth") 
cfg.MODEL.ROI_HEADS.SCORE_THRESH_TEST = 0.5
predictor = DefaultPredictor(cfg)


i = 0
for d in [file for file in os.listdir(test_dir) if not file.endswith('.json')]:    
    im = cv2.imread(test_dir +"/"+ d)
    outputs = predictor(im) 
    v = Visualizer(im[:, :, ::-1])
    out = v.draw_instance_predictions(outputs["instances"].to("cpu"))
    cv2.imwrite("../test_" + str(i) + ".jpg", out.get_image()[..., ::-1])
    i += 1

i = 0
for d in os.listdir(val_dir):    
    im = cv2.imread(val_dir + "/" + d)
    outputs = predictor(im)
    v = Visualizer(im[:, :, ::-1])
    out = v.draw_instance_predictions(outputs["instances"].to("cpu"))
    cv2.imwrite("../val_" + str(i) + ".jpg", out.get_image()[..., ::-1])
    i += 1