## Michi

'Michi' (道ーみち) means path. This program let's you control the actor by using commands that you enter in graphics console. It is also capable of displaying various internal information about the values and working of the system.

## Documentation
### Actions
* `move: <float>`
* `rotate: <float>`
* `enlarge: <float|vector2>`
* `change: <float|vector2|vector3|vector4>`
* `follow: <on|off>`
* `draw: <on|off>`
* `disp: <position|rotation|scale|color|speed|output|help|expr>`
* `exit`

### Variables
* `output` - dynamic variable
* `actor` - struct { position: vector2, rotation: float, scale: vector2, color: vector4 }
* `speed` - struct { position: float, rotation: float, scale: float, color: float }

### Syntax for setting the variables
```
variable: value
```

## Screenshot
![Screenshot](Screenshot.png)
