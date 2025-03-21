#!/usr/bin/env python3
import sys
import re
from transformers import pipeline, logging

def main():
    if len(sys.argv) < 2:
        print("Usage: converter.py <natural language query>")
        sys.exit(1)
    
    # Suppress extra logging messages.
    logging.set_verbosity_error()

    # Combine all command-line arguments into a single natural language query.
    nl_query = " ".join(sys.argv[1:])

    # Initialize the text-generation pipeline with TinyLlama.
    pipe = pipeline(
        "text-generation", 
        model="TinyLlama/TinyLlama-1.1B-Chat-v1.0", 
        torch_dtype="bfloat16", 
        device_map="auto"
    )

    # Build a refined prompt.
    prompt = (
        "You are a bash command generating chatbot. When given a natural language description of a task, output only a single valid bash command that accomplishes that task. "
        "Do not include any extra text, explanation, labels, or simulated output. Only output the command itself.\n\n"
        "Instruction: " + nl_query + "\n\nCommand:"
    )

    # Generate the response.
    outputs = pipe(
        prompt, 
        max_new_tokens=64,
        do_sample=True,
        temperature=0.7,
        top_k=50,
        top_p=0.95
    )

    generated_text = outputs[0]["generated_text"]

    # Remove the prompt portion if it is echoed.
    command_text = generated_text.replace(prompt, "").strip()

    # Remove any Markdown code block formatting (like "```bash" and "```").
    command_text = re.sub(r"```bash", "", command_text)
    command_text = re.sub(r"```", "", command_text)

    # Use regex to remove any stray special tokens.
    command_text = re.sub(r"<\|.*?\|>", "", command_text).strip()

    # In case the output still includes extra text, assume the command is on the first line.
    command_line = command_text.splitlines()[0].strip() if command_text else ""

    print(command_line)

if __name__ == "__main__":
    main()

