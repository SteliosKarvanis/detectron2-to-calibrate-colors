# Exame CT
## Implementacao da rede detectron2 para reconhecimento dos robos Very Small Size (VSS) 

Integrantes:

Stelios Karvanis

Renan Ferreira

## Instalação

### Modules
```
pip install torch
pip install opencv-python
```
### Detectron2

```
git clone https://github.com/facebookresearch/detectron2.git
python -m pip install -e detectron2
```

### Utilização

Para treinar o modelo, execute, da pasta root
```
python model/train_model.py
```

Para testar o modelo, execute 
```
python model/test_model.py
```

Para avaliar o modelo, execute
```
python model/evaluate.py
```
Caso a placa de vídeo não esteja configurada, é possível utilizar a cpu descomentando a linha com cfg.MODEL.DEVICE na função custom_cfg.

Os resultados dos testes estarão em dataset/test com o prefixo "output_".



