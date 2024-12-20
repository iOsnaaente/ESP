#include "micro_ros_platformio.h"
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_srvs/srv/set_bool.h>

#include "Arduino.h"
#include <math.h>


// Definição dos nós
rcl_node_t sqrt_check_node;
rcl_node_t idle_node;


// Serviço para verificar a raiz quadrada
rcl_service_t sqrt_check_service;
std_srvs__srv__SetBool_Request request_in;
std_srvs__srv__SetBool_Response response_out;


// Recursos de suporte
rcl_allocator_t allocator;
rclc_support_t support;
rclc_executor_t executor;


// Função callback do serviço
void sqrt_check_callback(const void *req, void *res) {
    const std_srvs__srv__SetBool_Request *request = (const std_srvs__srv__SetBool_Request *)req;
    std_srvs__srv__SetBool_Response *response = (std_srvs__srv__SetBool_Response *)res;

    // Calcula a raiz quadrada
    float number = request->data ? 1.0 : 0.0;
    float root = sqrtf(number);

    // Verifica se a raiz quadrada é um número inteiro
    if (root == floor(root)) {
        response->success = true;
        response->message.data = "Número possui uma raiz quadrada natural.";
    } else {
        response->success = false;
        response->message.data = "Número não possui uma raiz quadrada natural.";
    }
}

void setup() {
    
    // Configuração inicial do microROS
    Serial.begin(921600);
    set_microros_serial_transports( Serial );
  
    allocator = rcl_get_default_allocator();

    // Inicialização do suporte micro-ROS
    rclc_support_init(&support, 0, NULL, &allocator);

    // Criação do nó que verifica a raiz quadrada
    rclc_node_init_default(&sqrt_check_node, "sqrt_check_node", "", &support);

    // Inicialização do serviço
    rclc_service_init_default(
        &sqrt_check_service,
        &sqrt_check_node,
        ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool),
        "check_sqrt"
    );

    // Criação do nó que não faz nada
    rclc_node_init_default(&idle_node, "idle_node", "", &support);

    // Inicialização do executor
    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_service(&executor, &sqrt_check_service, &request_in, &response_out, sqrt_check_callback );
}

void loop() {
    // Executor processando chamadas de serviço
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    vTaskDelay( pdMS_TO_TICKS(100) );
}
