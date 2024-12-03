# OpenAI API Plugin
The **OpenAIApi Plugin** gives you access to the **OpenAI API** in **Unreal Engine**. It is compatible with at least  5.3 and 5.4

This is a community Plugin. No affiliation with OpenAI

## Installation

The recommended way to install the plugin is to use pre-built binaries.


- Download the Plugin [here](https://drive.google.com/drive/folders/16FFYDf0U--nxUocQVXCIvSo-Sa0Tnndl?usp=sharing)
- Create a `Plugins` folder under your project folder
- Copy the plugin to it, so you have `YourProject/Plugins/OpenAIAPI`

## Quick Start

See [Video](https://www.youtube.com/watch?v=hUv2_gis_9I)

[![](http://img.youtube.com/vi/hUv2_gis_9I/0.jpg)](http://www.youtube.com/watch?v=hUv2_gis_9I "OpenAI API Quick Start Tutorial")

> **NOTE**: This tutorial is out of date. Updating it soon to show how you can use this plugin to: <br /> Generate images at runtime using `DALL·E 2`. <br /> Transcribe speech in any language using `Whisper v2-large`. <br /> Use the chat endpoint to power an NPC with `GPT-4` and `GPT-3.5-Turbo`.

## Types of Authentication
There are two ways to set the Api Key
- Assign the Api key directly in blueprints.

- Set the Api Key as an environment variable


![](https://i.imgur.com/HF2tdBz.png)
> **ADVICE**: Pay attention to security and encrypt your assets before packaging.


![](https://i.imgur.com/0fpPVlV.png)
> **ADVICE**: Create the environment variable `OPENAI_API_KEY` and use your Api key as the value.


## Blueprint Nodes
#### OpenAI Call Completions

- This async node sends an HTTP request using your `engine`, `prompt`, and `settings`.

![](https://i.imgur.com/vGo2wta.png)
> **NOTE**: Async nodes can only be called from a blueprint's `Event Graph`. 
#### Make Completion Settings

- This node allows you to set the `Request body` parameters.

![](https://i.imgur.com/xS4MMrI.png)
> **NOTE**: The extra parameter `Start Sequence` injects text at the beginning of your prompt.
>
> `stream` will be supported soon
#### Break Completion

- This node is used to access the values included in a `completion` or `choice`

![](https://i.imgur.com/dydM8Sd.png)

## Usage
This example shows OpenAI's OpenAI's completions endpoint in blueprints. (GPT-3)

![](https://i.imgur.com/DNKp0bW.png)

This example show you how to use OpenAI's chat endpoint in blueprints. (GPT-4 / GPT-3.5-Turbo)

![](https://i.imgur.com/hHFSp4b.png)

This example shows you how to generate a 1024x1024 image using DALL·E 2 in blueprints.

![](https://i.imgur.com/CciUUF6.png)

This example shows you how to record and transcribe speech using Whisper v2-Large in blueprints.

![](https://i.imgur.com/ameqz1L.png)

## Blueprint for Realtime Voice API using the Runtime Audio Importer plugin
![image](https://github.com/user-attachments/assets/18564ac6-d333-4566-ae83-1671c1cc2c92)

## References
- [OpenAI's API Engine Documentation](https://beta.openai.com/docs/engines)
- [OpenAI's API Reference](https://beta.openai.com/docs/api-reference/completions)

## Supported Platforms
Windows, Mac*, Android*

  *Realtime Voice API mic audio capture has only been tested on Windows
