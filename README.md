# QtKinectInteraction

### Version 1.0 Beta

## Demo(Nov. 10, 2017)
[Demo](Demo/demo20171110.mp4)

## Introduction
This is an improved Qt Version of our [project](https://github.com/zkyf/Kinect-Interaction-Tool). QtKinectInteraction is a tool for interaction with Windows applications by sending keyboard and mouse to the OS. User can specify their own gestures and meanings of the gestures.

Theoretically, this can be used for any Windows application and can achieve some accurate operation, though this may need some practice.

To use this system, you just need to draw your own gesture templates, write the config file, and then try some interaction using kinect.

## Gesture
The gesture here is a continuous track of closed hand. To start a gesture, the user just needs to close your hand, and draw a curve, and then open the hand. Then, the system will compare the gesture with all the available templates specified in the config file, and send corresponding commands to the OS as written in the config file.

## Features
* Support user-specified gestures and configuration file.
* Support accurate mouse control.

## Requirement
* [Kinect SDK v2.0](https://www.microsoft.com/en-us/download/details.aspx?id=44561)
* [Qt 5.8.0](https://www.qt.io/)

## Config File Grammar
Config file defines a modified [Finite State Machine](https://en.wikipedia.org/wiki/Finite-state_machine), which allows user to transfer to other states for different situation or application, for example, assigning different commands in multiple situations to the same gesture.

The FSM is a directed graph with finite state nodes, and some directed edges called transitions. Each state is a node in the graph, and usually with some actions, or outputs. Each transition is a directed edge connnecting two nodes with a transition condition. Here we use states for different utility environment, and transitions for user gestures.

FSM starts from an origin node, and transfers to other states according to the input and the tansitions. What's different here is that we put actions on the transitions instead of nodes, because this is a interaction application, so transitions should correspond to all the actions, but not the states.

All the nodes should be declared first using `ADD state_name`. By default, there is always an origin node named ORIGIN as the starting state of the FSM. A typical transition sentence is:

```
[state source_state] [hand left|right] template a_gesture_file_name action_part target_state
```

In this sentence, source state of the transition will be ORIGIN if not specified using `state source_state` sub-sentence.

`hand` sub-sentence specifies the hand to be used for this gesture. By default, right hand will be used.

`a_gesture_file_name` is the file name of the gesture template file.

`action_part` specifies the corresponding action for this gesture. Several kinds of actions are supported.

* **Key/KeyUp/KeyDown**: these actions generate corresponding keyboard events using [SendInput](https://msdn.microsoft.com/en-us/library/windows/desktop/ms646310%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396) methods. Following Key/KeyUp/KeyDown is the [Virtual Key Codes](https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx) of the keys in integer format. Eg. `Key 69` means to click key E when the action is executed.
* **MouseWheel**: this sends the amount of mouse wheel movement. This can be used to control slides and scroll bars on the page.
* **Program**: this will execute the following command using `system()` function.
`target_state` specifies the target state of the transition, and cannot be ignored.

Besides sentences above, there's another `cursor some_state left|right` sentence to enable mouse controlling using the specified hand in the `some_state`. While using, the mouse will be moveing to the same direction as the hand relative to the corresponding shoulder. Eg. if `cursor origin left` is specified, the cursor will move to its left if the left hand is on your left, and the cursor will move to its right if it's on the right to the left shoulder. The larger the distance between the hand and the shoulde is, the faster the cursor moves.

In the config file, comments can be added after `# `. Note that there's always a SPACE after #.

Note that config file is not upper case sensitive. That is, `oRigIN` is the same as `OrIgIn`.

## Gesture Template Files
Gesture template files(templates) are in fact a text file saving the coordinates of all the 2D points of a gesture. The system uses [dollar one](http://depts.washington.edu/madlab/proj/dollar/index.html) algorithm to recognize gestures.

## In-coming
* More comfortable mouse cursor speed
* Bug fixing

## Release Notes
### Nov. 11, 2017
Release the first version of the app.
