# Atyu QMK Fork

This is a fork of QMK used by Atyu to configure OLED mods onto keyboards.

## Supported Keyboards

All supported keyboards are configured in the [atyu_home](/atyu_home.json) config.

## Documentation

TODO

## Atyu for Devs

The Atyu app reads a few configs that need to be updated in order to add mods to a certain keyboard:

- atyu_config.json:

This is responsible for the UI that is constructed by the Atyu app. To add an option or mod visible and controllable by the app, you will need to add a section in  here. The schemas are defined [here](https://github.com/atude/atyu-app/blob/master/src/configs/atyuConfig.ts). The sections are split into top level sections that can only enable/disable flags, and child sections that allow for multiselecting flags, setting number values to flags, and more.


Top level config:

| Key | Description | Values 
------|---------------|---------
| `name` | The title of the mod | string
| `desc` | A short subtitle describing what the mod does | string
| `key` | The key that will be generated in `atyu.h`. This is the flag you can use to enable/disable your mod within QMK. | `string`
| `configurable` | Whether this is an option that the user should be able to change. This will almost always be set to `true` | `boolean`
| `enabledByDefault` | Whether the flag is enabled by default. This will almost always be set to `false` | `boolean`
| `notes` | Notes that describe some interactions with your mod. These will show up on the tooltip as a list of bullet points | `string[]`
| `children` | The subcomponents of this mod. More information about this in the child config section below | Array of child components


For example, to add an option to enable a flag called OLED_ENABLE_SOME_MODE, you will need to add a section like:
```js
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

WIP


