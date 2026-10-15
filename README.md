# IoT / Firmware - ESP32

Neste repositório, está o código do sistema embarcado (ESP32) responsável por coletar as medidas de qualidade do ar (temperatura, umidade e CO2) e enviá-las via MQTT para o backend Java do projeto.

## Resumido
A ideia é que o ESP32 faça a leitura dos sensores do meio ambiente e publique os dados em um broker MQTT, seguindo os contratos estabelecidos para integração com o servidor.