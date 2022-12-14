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
python train_model.py
```

Para testar o modelo, execute 
```
python test_model.py
```

Os resultados dos testes estarão em dataset/test com o prefixo "output_".



