import cv2
from ultralytics import YOLO
from ultralytics.utils.plotting import Annotator

video_cap = cv2.VideoCapture("SEU_VIDEO_AQUI") # abre um video

model = YOLO("yolov8m.pt") # modelo YOLO

while True:
    ret, frame = video_cap.read() # leitura do video
    controlkey = cv2.waitKey(1)
    if not ret: # termina loop se nao houver quadro
        break
    tracker = model.track(frame, persist=True, classes=[1, 2, 3, 5, 6, 7]) # aplica algoritmo buscando as classes indicadas
    for r in tracker: # anota todos os objetos rastreados
        annotator = Annotator(frame)
        boxes = r.boxes # caixas delimitadoras dos objetos rastreados
        for box in boxes:
            b = box.xyxy[0]  # coordenadas de cada caixa delimitadora
            c = box.cls # classes identificadas
            annotator.box_label(b, model.names[int(c)]) # anota quadro com caixa e classe
    cv2.imshow("Frame", frame) # mostra quadro anotado
    if cv2.waitKey(1) == ord("q"): # comando para sair do loop
        break
video_cap.release()
cv2.destroyAllWindows()