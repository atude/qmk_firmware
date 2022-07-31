# Atyu QMK Fork

This is a fork of QMK used by Atyu to configure OLED mods onto keyboards.

## Supported Keyboards

All supported keyboards are configured in the [atyu_home](/atyu_home.json) config. To add support for a keyboard, add a config for that file. Examples on how to make it can be taken from the file itself.

## Documentation

TODO

## Atyu for Devs

The Atyu app reads a few configs that need to be updated in order to add mods to a certain keyboard:

### atyu_config.json

This is responsible for the UI that is constructed by the Atyu app. To add an option or mod visible and controllable by the app, you will need to add a section in  here. The schemas are defined [here](https://github.com/atude/atyu-app/blob/master/src/configs/atyuConfig.ts). The sections are split into top level sections that can only enable/disable flags, and child sections that allow for multiselecting flags, setting number values, and more.


##### Top level config
| Key | Description | Values | Necesaary field? |
------|---------------|---------|--------------
| `name` | The title of the mod | `string` | yes
| `desc` | A short subtitle describing what the mod does | `string` | yes
| `key` | The key that will be generated in `atyu.h`. This is the flag you can use to enable/disable your mod within QMK. | `string` | yes
| `configurable` | Whether this is an option that the user should be able to change. This will almost always be set to `true` | `boolean` | yes
| `enabledByDefault` | Whether the flag is enabled by default. This will almost always be set to `false` | `boolean` | yes
| `notes` | Notes that describe some interactions with your mod. These will show up on the tooltip as a list of bullet points | `string[]` | no
| `children` | The subcomponents of this mod. More information about this in the child config section below | Array of child configs | yes

##### Child config
| Key | Description | Values | Necesaary field? |
------|---------------|---------|----------
| `name` | The title of the child section | `string` | yes
| `desc` | A short subtitle describing what the mod does | `string` | no
| `struct` | The structure of options for this section | `MultiselectBoolean`, `RadioNumber` or `Switch` | yes

The child config can have the following options:
- Multiselect boolean: A list of flags that the user can enable or disable
- Radio number: A list of numbers that the user can set a flag to
- Switch: A switch that for the user to enable/disable a flag

##### MultiselectBoolean option
| Key | Description | Values | Necesaary field? |
------|---------------|---------|----------
| `type` | N/A | The string `multiselect_boolean` | yes
| `multiselectStruct` | An array of values for a set of flags you would like the user to choose. `key` should represent the flag that this multiselect option enables/disables | `{ name: string, key: string, defaultValue: boolean }[]` | yes
| `multiselectOptions` | Optional settings for limits on your multiselect section | `{ min: int, max: int }` | no

##### RadioNumber option
| Key | Description | Values | Necesaary field? |
------|---------------|---------|----------
| `type` | N/A | The string `radio_number` | yes
| `radioKey` | The flag that you will be setting a number value to | `string` | yes
| `defaultValue` | The default number value the flag is set to | `int` | yes
| `radioValues` | A list of options that the user can set this flag to | `{ name: string, value: int }[]` | yes

##### Switch option
| Key | Description | Values | Necesaary field? |
------|---------------|---------|----------
| `type` | N/A | The string `switch` | yes
| `key` | The flag that the user can enable/disable | `string` | yes
| `defaultValue` | The default boolean value the flag is set to | `boolean` | yes

#### Examples

To add an option to enable a flag called `OLED_ENABLE_SOME_MODE`, you will need to add a section like:
```json
{
    "name": "Some OLED mode",
    "desc": "This enables some oled mode which does cool things",
    "key": "OLED_ENABLE_SOME_MODE",
    "configurable": true,
    "enabledByDefault": true,
    "children": [],
    "notes": [
        "You can do another cool thing if you do X or Y"
    ]
}
```

Example bongo cat config, with a child config to enable/disable the filled bongo cat mode:
```json
{
    "name": "Bongo Cat",
    "desc": "Bongo cat that taps when you type and interacts with mod keys",
    "key": "OLED_BONGO_ENABLED",
    "configurable": true,
    "enabledByDefault": false,
    "children": [
        {
            "name": "Use filled bongo cat",
            "struct": {
                "type": "switch",
                "key": "OLED_BONGO_FILLED",
                "defaultValue": false
            }
        }
    ],
    "notes": [
        "Also shows time, WPM, current layer and encoder mode",
        "Interacts on caps lock, ctrl, winkey and OLED timeout",
        "Switch between time, date, and no text using F22 (mappable in VIA)"
    ]
}
```

Example polling config that lets a user choose from a list of preset polling rates (note the `key` is a dummy value here, and doesn't need to be used in QMK):
```json
{
    "name": "Polling",
    "desc": "",
    "key": "__keyboard_options",
    "configurable": false,
    "enabledByDefault": true,
    "children": [
        {
            "name": "Polling rate",
            "desc": "Sets typing responsiveness (lower is faster)",
            "struct": {
                "type": "radio_number",
                "radioKey": "KB_OPTIONS_POLLING_RATE",
                "defaultValue": 1,
                "radioValues": [
                    {
                        "name": "1ms",
                        "value": 1
                    },
                    {
                        "name": "2ms",
                        "value": 2
                    },
                    {
                        "name": "5ms",
                        "value": 5
                    },
                    {
                        "name": "10ms",
                        "value": 10
                    }
                ]
            }
        }
    ]
}
```

WIP


