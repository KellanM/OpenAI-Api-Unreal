# OpenAI API Plugin
The **OpenAIApi Plugin** gives you access to the **OpenAI API** in **Unreal Engine**. It is compatible with 4.26, 4.27, 5.0, and 5.1

This is a community Plugin. No affiliation with OpenAI

## Installation

The recommended way to install the plugin is to use pre-built binaries.


- Download the Plugin [here](https://drive.google.com/drive/folders/16FFYDf0U--nxUocQVXCIvSo-Sa0Tnndl?usp=sharing)
- Create a `Plugins` folder under your project folder
- Copy the plugin to it, so you have `YourProject/Plugins/OpenAIAPI`

## Quick Start

See [Video](https://www.youtube.com/watch?v=hUv2_gis_9I)

[![](http://img.youtube.com/vi/hUv2_gis_9I/0.jpg)](http://www.youtube.com/watch?v=hUv2_gis_9I "OpenAI API Quick Start Tutorial")

> **NOTE**: This video will soon be updated to show you how to generate images in `Unreal Engine` with `DALL·E 2`. 

## Types of Authentication
There are two ways to set the Api Key
- Assign the Api key directly in blueprints.

- Set the Api Key as an environment variable


![](https://i.imgur.com/HF2tdBz.png)
> **ADVICE**: Pay attention to security and encrypt your assets before packaging.


![](https://i.imgur.com/0fpPVlV.png)
> **ADVICE**: Create the environment variable `OPENAI_API_KEY` and use your Api key as the value.


## Blueprint Nodes
#### OpenAI Call GPT3

- This async node sends an HTTP request using your `engine`, `prompt`, and `settings`.

![](https://i.imgur.com/vGo2wta.png)
> **NOTE**: This node can only be called from a blueprint's `Event Graph`. 
#### Make Settings

- This node allows you to set the `Request body` parameters.

![](https://i.imgur.com/xS4MMrI.png)
> **NOTE**: The extra parameter `Start Sequence` injects text at the beginning of your prompt.
>
> `log_probs` and `stream` will be supported soon
#### Break Completion

- This node is used to access the values included in a `completion` or `choice`

![](https://i.imgur.com/dydM8Sd.png)
> **NOTE**: `log_probs` is currently unsupported.
## Usage
This example shows OpenAI's chat example implemented in blueprints.

![](https://i.imgur.com/DNKp0bW.png)

This example shows how you can generate a 1024x1024 image using DALL·E 2 in blueprints.

![](https://i.imgur.com/CciUUF6.png)

## References
- [OpenAI's API Engine Documentation](https://beta.openai.com/docs/engines)
- [OpenAI's API Reference](https://beta.openai.com/docs/api-reference/completions)

## Supported Platforms
Windows, Mac, Android
