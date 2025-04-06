#!/usr/bin/env python3
import sys
import re
import google.generativeai as genai
import os

def main():
    if len(sys.argv) < 2:
        print("Usage: converter.py <natural language query>")
        sys.exit(1)

    # Hardcode API key directly - ONLY FOR TESTING, not recommended for production
    api_key = "AIzaSyCQzED8htpL-v-hkXioQTNxaTff62SI4Io"  # Replace with your actual key
    
    if not api_key or api_key == "YOUR_ACTUAL_GOOGLE_API_KEY_HERE":
        print("Please edit the script and replace the api_key value with your actual Google API key")
        sys.exit(1)

    # Combine command-line arguments
    nl_query = " ".join(sys.argv[1:])

    try:
        # Configure the Gemini API
        genai.configure(api_key=api_key)
        
        # Initialize the Gemini model
        model = genai.GenerativeModel('gemini-2.0-flash')
        
        # Build the prompt
        prompt = (
            "You are a bash command generating chatbot. When given a natural language description of a task, output only a single valid bash command that accomplishes that task. "
            "Do not include any extra text, explanation, labels, or simulated output. Only output the command itself.\n\n"
            "Instruction: " + nl_query + "\n\nCommand:"
        )

        # Generate response using Gemini
        response = model.generate_content(prompt)
        
        # Extract the command from response
        command_text = response.text.strip()
        command_text = re.sub(r"```bash", "", command_text)
        command_text = re.sub(r"```", "", command_text)
        command_line = command_text.splitlines()[0].strip() if command_text else ""

        print(command_line)
        
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()