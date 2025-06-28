import cv2
import numpy as np
from ultralytics import YOLO

# Carrega o modelo YOLO pré-treinado
model = YOLO('yolov8n.pt')

# Abre webcam (0 = primeira webcam)
cap = cv2.VideoCapture(0)

# Classes de veículos a considerar
vehicle_classes = ['car', 'truck', 'bus', 'motorcycle']

# Lê um frame para saber as dimensões
ret, frame = cap.read()
if not ret:
    print("Erro ao acessar a câmera.")
    cap.release()
    exit()

height, width = frame.shape[:2]

# Define área de contagem = tela toda
area_pts = np.array([[0, 0], [width, 0], [width, height], [0, height]])

print(f"Área de contagem: {area_pts}")

while cap.isOpened():
    success, frame = cap.read()
    if not success:
        break

    # Detecta + rastreia
    results = model.track(frame, persist=True, show=False)
    annotated_frame = results[0].plot()

    # IDs únicos dentro da área neste frame
    current_ids_in_area = set()

    for box in results[0].boxes:
        cls_id = int(box.cls[0])
        class_name = model.names[cls_id]

        if class_name in vehicle_classes:
            track_id = int(box.id[0]) if box.id is not None else None

            # Coordenadas do bbox
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            center = (int((x1 + x2) / 2), int((y1 + y2) / 2))

            # Testa se o centro está dentro da área
            inside = cv2.pointPolygonTest(area_pts, center, False) >= 0

            print(f"Classe: {class_name}, Centro: {center}, Inside: {inside}")

            if inside and track_id is not None:
                current_ids_in_area.add(track_id)

            # Desenha centro do bbox
            cv2.circle(annotated_frame, center, 7, (0, 255, 255), -1)

    # Contagem = quantos IDs únicos dentro da área agora
    vehicle_count = len(current_ids_in_area)

    # Desenha polígono da área (opcional: para tela toda, nem precisa)
    cv2.polylines(annotated_frame, [area_pts], isClosed=True, color=(0, 0, 255), thickness=2)

    # Escreve contagem na tela
    cv2.putText(annotated_frame, f'Veiculos na area: {vehicle_count}', (50, 50),
                cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)

    # Mostra vídeo
    cv2.imshow('Contagem de Veiculos', annotated_frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Libera recursos
cap.release()
cv2.destroyAllWindows()
