from constants import *
from utils import *


cfg = custom_cfg()
DatasetCatalog.register("test", lambda : get_data(test_dir))
MetadataCatalog.get("test").set(thing_classes=classes)
test_metadata = MetadataCatalog.get("test")
cfg.MODEL.WEIGHTS = os.path.join(cfg.OUTPUT_DIR, "model_final.pth") 
cfg.MODEL.ROI_HEADS.SCORE_THRESH_TEST = 0.5
predictor = DefaultPredictor(cfg)


for index, filename in enumerate([file for file in os.listdir(test_dir) if not (file.endswith('.json') or file.startswith('output'))]):
    im = cv2.imread(test_dir +"/"+ filename)
    outputs = predictor(im) 
    v = Visualizer(im[:, :, ::-1], metadata=test_metadata) 
    out = v.draw_instance_predictions(outputs["instances"].to("cpu"))
    cv2.imwrite(test_dir + "/output_" + filename, out.get_image()[..., ::-1])
