import cv2
import numpy as np
from ultralytics import YOLO

model = YOLO('yolov8n.pt')

area_pts = np.array([[200, 300], [500, 300], [500, 500], [200, 500]])

cap = cv2.VideoCapture(0)

vehicle_classes = ['car', 'truck', 'bus', 'motorcycle']

while cap.isOpened():
    success, frame = cap.read()
    if not success:
        break

    results = model.track(frame, persist=True, show=False)
    annotated_frame = results[0].plot()

    current_ids_in_area = set()

    for box in results[0].boxes:
        cls_id = int(box.cls[0])
        class_name = model.names[cls_id]

        if class_name in vehicle_classes:
            track_id = int(box.id[0]) if box.id is not None else None

            x1, y1, x2, y2 = map(int, box.xyxy[0])
            center = (int((x1 + x2) / 2), int((y1 + y2) / 2))

            inside = cv2.pointPolygonTest(area_pts, center, False) >= 0

            if inside and track_id is not None:
                current_ids_in_area.add(track_id)

            cv2.circle(annotated_frame, center, 5, (0, 255, 0), -1)

    vehicle_count = len(current_ids_in_area)

    cv2.polylines(annotated_frame, [area_pts], isClosed=True, color=(0, 0, 255), thickness=2)

    cv2.putText(annotated_frame, f'Veiculos na area: {vehicle_count}', (50, 50),
                cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)

    cv2.imshow('Contagem de Veiculos', annotated_frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
