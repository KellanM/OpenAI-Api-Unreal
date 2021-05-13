# OpenAI-Api-Unreal
A packaged plug-in that integrates the OpenAI Api in Unreal Engine

This is a community library. No affiliation with OpenAI

Contributions are welcome!

# Quick Start
See video: (under construction)

## Install

1. Download the Plug-in
2. Navigate to your project's root directory. Create a new folder named `Plugins` (Skip this step if one already exists.)
![](https://i.imgur.com/7mYEjNN.png)
3. Extract the `OpenAIAPI` plug-in into your project's `Plugins` folder
4. Open your `.uproject`
5. From within the editor, navigate to `Edit > Plugins`
![](https://i.imgur.com/IMLpdIP.png)
6. Confirm the `OpenAIAPI` plug-in is enabled
![](https://i.imgur.com/XyHq9UE.png)

# Types of Authentication


## Set Directly in Blueprints

Assign the key directly in blueprints. This can be called from anywhere.
![](https://i.imgur.com/HF2tdBz.png)

## Set as Environment Variable

Create the environment variable `OPENAI_API_KEY` and use your Api key as the value.

By default you will need to set your Api key directly in blueprints. To use your environment variable, you will need to set `Use Env Variable` to `true`.
![](https://i.imgur.com/0fpPVlV.png)

> **ADVICE**: Pay attention to security and encrypt your assets before packaging.

# Blueprint Nodes
### OpenAI Call GPT3

An async node that sends an HTTP request using the provided `engine`, `prompt`, and `settings`.

Please refer to [OpenAI's API Engine Documentation](https://beta.openai.com/docs/engines)  for information about each `engine`. 

![](https://i.imgur.com/vGo2wta.png)

> **NOTE**: This node can only be called from a blueprint's `Event Graph`. 
> 
> The `Finished` exec pin will fire once a response is received.

### Make Settings

This node creates a struct that includes the parameters involved in the HTTP `Request body`.

Hovering over an input pin will bring up a `tooltip` that provides information about the given parameter.

For more information about each parameter, please refer to [OpenAI's API Reference](https://beta.openai.com/docs/api-reference/completions).
![](https://i.imgur.com/xS4MMrI.png)
> **NOTE**: the `log_probs` and `stream` parameters are currently unsupported.
>
> The extra parameter `Start Sequence` injects text at the beginning of your prompt.
### Break Completion

This node is used to access the values included in a `completion` (Referred to as 'choice' in OpenAI's Documentation).
![](https://i.imgur.com/dydM8Sd.png)
> **NOTE**: `log_probs` is currently unsupported. Its default value is set to 0.
## Example Usage
Here is an example implementation of OpenAI's chat preset.
![](https://i.imgur.com/DNKp0bW.png)

## Compatability
This plug-in has been tested with UE4.26

## Supported Platforms
Windows, Mac, Android
