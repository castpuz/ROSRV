#!/usr/bin/env python2

import rospy as ros
from subprocess import Popen, check_call
from std_msgs.msg import String

import pytest

@pytest.fixture(scope="session")
def ros_init():
    ros.init_node('test')

@pytest.fixture()
def ros_subscribe(ros_init):
    def subscribe(topic):
        class context: # see https://stackoverflow.com/a/28433571/1278288
            recieved = []
        def callback(msg):
            ros.loginfo("Recieved: %s", msg.data)
            context.recieved += [msg.data]
        rate = ros.Rate(10) # Hz
        ros.Subscriber(topic, String, callback)
        rate.sleep()
        return [ros_init, rate, context.recieved]
    return subscribe

@pytest.fixture()
def launch_monitor(request):
    def launch(name):
        monitor = Popen(['/usr/bin/env', 'rosrun', 'rvmonitor', name])
        def cleanup():
            monitor.terminate()
        request.addfinalizer(cleanup)
        return monitor
    return launch

## Tests without RVMaster

def test_roscore__runmonitored_channel(ros_subscribe):
    [ros_init, rate, recieved_messages] = ros_subscribe('unmonitored')
    check_call(['rostopic', 'pub', '--once', '/unmonitored', 'std_msgs/String', 'Hi!'])
    rate.sleep(); rate.sleep(); rate.sleep(); rate.sleep()
    assert(recieved_messages == ['Hi!'])

def test_roscore__rmonitored_channel__no_monitor_running(ros_subscribe):
    [ros_init, rate, recieved_messages] = ros_subscribe('monitored')
    check_call(['rostopic', 'pub', '--once', '/chatter', 'std_msgs/String', 'Hi!'])
    rate.sleep(); rate.sleep(); rate.sleep(); rate.sleep()
    assert(recieved_messages == [])

def test_roscore__rmonitored_channel__monitor_running(ros_subscribe, launch_monitor):
    [ros_init, rate, recieved_messages] = ros_subscribe('chatter')
    launch_monitor('monitor-single-parameter')
    check_call(['rostopic', 'pub', '--once', '/rv/monitored/chatter', 'std_msgs/String', 'Hi!'])
    check_call(['rostopic', 'pub', '--once', '/rv/monitored/chatter', 'std_msgs/String', 'Hi!'])
    rate.sleep(); rate.sleep(); rate.sleep(); rate.sleep()
    assert(recieved_messages == ['Hi!RV1', 'Hi!RV2'])

def test_roscore__rmonitored_channel__monitor_running(ros_subscribe, launch_monitor):
    [ros_init, rate, recieved_messages] = ros_subscribe('chatter')
    launch_monitor('monitor-single-parameter')
    check_call(['rostopic', 'pub', '--once', '/rv/monitored/chatter', 'std_msgs/String', 'Hi!'])
    check_call(['rostopic', 'pub', '--once', '/rv/monitored/chatter', 'std_msgs/String', 'Hi!'])
    rate.sleep(); rate.sleep(); rate.sleep(); rate.sleep()
    assert(recieved_messages == ['Hi!RV1', 'Hi!RV2'])

