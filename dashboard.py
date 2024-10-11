import dash
from dash import dcc, html
from dash.dependencies import Input, Output
import paho.mqtt.client as mqtt
import json
import threading

# Variáveis globais para armazenar os dados dos sensores
temperature = 0
humidity = 0
ldr = 0

# Função chamada quando uma mensagem MQTT é recebida
def on_message(client, userdata, msg):
    global temperature, humidity, ldr
    data = json.loads(msg.payload.decode())  # Decodifica a mensagem JSON
    temperature = data.get('temperature', 0)
    humidity = data.get('humidity', 0)
    ldr = data.get('ldr', 0)
    print(f"Recebido -> Temperatura: {temperature}, Umidade: {humidity}, Luminosidade: {ldr}")

# Configuração do cliente MQTT
def mqtt_client():
    client = mqtt.Client()
    client.on_message = on_message
    client.connect("46.17.108.131", 1883, 60)  # Conecta ao broker MQTT
    client.subscribe("/TEF/sensor001/attrs")   # Inscreve-se no tópico MQTT
    client.loop_forever()                      # Inicia o loop para receber mensagens

# Inicia o cliente MQTT em uma thread separada
mqtt_thread = threading.Thread(target=mqtt_client)
mqtt_thread.daemon = True
mqtt_thread.start()

# Configuração do dashboard usando Dash
app = dash.Dash(__name__)

# Layout do dashboard
app.layout = html.Div([
    html.H1("Monitoramento de Sensores em Tempo Real"),
    
    # Seção para exibir as métricas ao vivo
    html.Div(id='live-update-text', style={'fontSize': 20}),
    
    # Gráfico que será atualizado em tempo real
    dcc.Graph(id='live-update-graph'),
    
    # Intervalo para atualizar o dashboard a cada 1 segundo
    dcc.Interval(
        id='interval-component',
        interval=1*1000,  # 1000 milissegundos = 1 segundo
        n_intervals=0
    )
])

# Callback para atualizar o texto com as últimas leituras dos sensores
@app.callback(Output('live-update-text', 'children'),
              Input('interval-component', 'n_intervals'))
def update_metrics(n):
    return [
        html.P(f"Temperatura: {temperature:.2f} °C"),
        html.P(f"Umidade: {humidity:.2f} %"),
        html.P(f"Luminosidade (LDR): {ldr:.2f} %")
    ]

# Callback para atualizar o gráfico em tempo real
@app.callback(Output('live-update-graph', 'figure'),
              Input('interval-component', 'n_intervals'))
def update_graph_live(n):
    # Define os dados do gráfico de barras
    fig = {
        'data': [
            {'x': ['Temperatura'], 'y': [temperature], 'type': 'bar', 'name': 'Temperatura'},
            {'x': ['Umidade'], 'y': [humidity], 'type': 'bar', 'name': 'Umidade'},
            {'x': ['Luminosidade'], 'y': [ldr], 'type': 'bar', 'name': 'Luminosidade'}
        ],
        'layout': {
            'title': 'Dados dos Sensores em Tempo Real'
        }
    }
    return fig

# Inicia o servidor do dashboard
if __name__ == '__main__':
    app.run_server(debug=True)
