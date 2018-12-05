from sympy import cos, sin, symbols, lambdify, diff
import numpy as np
import json
import socket
from is_msgs.robot_pb2 import RobotTaskRequest, RobotTaskReply, RobotControllerProgress
from is_msgs.common_pb2 import Position, Speed
from is_wire.core import Channel, Message, Subscription


def lemniscate_of_bernoulli(shape,
                            center,
                            lap_time,
                            rate=10.0,
                            allowed_error=0.1):
    t = symbols('t')
    w = 2 * np.pi / lap_time
    phi = np.pi / 2

    ax = (2 * shape[0]) / (3 - cos(2 * (w * t + phi)))
    ay = (shape[1] / 0.35) / (3 - cos(2 * (w * t + phi)))

    x = ax * cos((w * t + phi)) / 2 + center[0]
    y = ay * sin(2 * (w * t + phi)) / 2 + center[1]

    dx = diff(x, t)
    dy = diff(y, t)

    fx = lambdify((t), x, 'numpy')
    fy = lambdify((t), y, 'numpy')
    dfx = lambdify((t), dx, 'numpy')
    dfy = lambdify((t), dy, 'numpy')

    t = np.arange(0, lap_time, 1.0 / rate)

    task = RobotTaskRequest()
    task.basic_move_task.positions.extend(
        [Position(x=x, y=y) for x, y in zip(fx(t), fy(t))])
    task.basic_move_task.speeds.extend(
        [Speed(linear=l, angular=w) for l, w in zip(dfx(t), dfy(t))])
    task.basic_move_task.rate = rate
    task.basic_move_task.allowed_error = allowed_error
    return task


def final_position(target, rate=10, allowed_error=0.1):
    task = RobotTaskRequest()
    task.basic_move_task.positions.extend([Position(x=target[0], y=target[1])])
    task.basic_move_task.rate = rate
    task.basic_move_task.allowed_error = allowed_error
    return task


with open("../etc/conf/options.json") as f:
    options = json.load(f)

task = lemniscate_of_bernoulli(shape=(2, 1), center=(0, 0), lap_time=20)
#task = final_position(target=(0, 0))

channel = Channel(options["broker_uri"])
subscription = Subscription(channel)
prefix = "RobotController.{}".format(options["parameters"]["robot_id"])

message = Message(content=task, reply_to=subscription)
channel.publish(message, topic="{}.SetTask".format(prefix))
reply = channel.consume(timeout=1.0)
if not reply.status.ok():
    raise Exception(reply.status.why)
print(reply.unpack(RobotTaskReply))

progress_topic = "{}.Progress".format(prefix)
subscription.subscribe(progress_topic)
while True:
    message = channel.consume()
    if message.topic == progress_topic:
        print(message.unpack(RobotControllerProgress))
