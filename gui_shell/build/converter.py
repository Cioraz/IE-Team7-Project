#!/usr/bin/env python3
import sys
import re
import torch
from transformers import AutoTokenizer, AutoModelForCausalLM, logging
from peft import PeftModel, PeftConfig

def main():
    if len(sys.argv) < 2:
        print("Usage: converter.py <natural language query>")
        sys.exit(1)

    # Suppress logging
    logging.set_verbosity_error()

    # Combine command-line arguments
    nl_query = " ".join(sys.argv[1:])

    try:
        # First, load the PEFT config to get the base model info
        peft_config = PeftConfig.from_pretrained("./checkpoint-1040")
        base_model_name = peft_config.base_model_name_or_path
        
        # If the config doesn't have the base model name, use the hardcoded value
        if not base_model_name or base_model_name == "":
            base_model_name = "TinyLlama/TinyLlama-1.1B-Chat-v1.0"
            
        #print(f"Using base model: {base_model_name}")
        
        # Load tokenizer
        tokenizer = AutoTokenizer.from_pretrained(base_model_name)
        
        # Load base model with specific device configuration
        #print("Loading base model...")
        # Use CPU if CUDA is not available to avoid disk offloading issues
        device_map = "cpu" if not torch.cuda.is_available() else "auto"
        
        base_model = AutoModelForCausalLM.from_pretrained(
            base_model_name,
            torch_dtype=torch.float32,  # Use float32 for CPU compatibility
            device_map=device_map,
            offload_folder=None,  # Disable offloading
            offload_state_dict=False  # Disable state dict offloading
        )
        
        # Load adapter
        #print("Loading adapter...")
        model = PeftModel.from_pretrained(
            base_model, 
            "./checkpoint-1040",
            is_trainable=False
        )
        model.eval()
        
        #print("Model loaded successfully")
    except Exception as e:
        print(f"Error loading model: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

    # Build the prompt
    prompt = (
        "You are a bash command generating chatbot. When given a natural language description of a task, output only a single valid bash command that accomplishes that task. "
        "Do not include any extra text, explanation, labels, or simulated output. Only output the command itself.\n\n"
        "Instruction: " + nl_query + "\n\nCommand:"
    )

    # Tokenize input
    input_ids = tokenizer(prompt, return_tensors="pt").input_ids.to(model.device)

    # Generate output
    with torch.no_grad():
        outputs = model.generate(
            input_ids,
            max_new_tokens=64,
            do_sample=True,
            temperature=0.7,
            top_k=50,
            top_p=0.95,
            pad_token_id=tokenizer.eos_token_id
        )

    # Decode output
    generated_text = tokenizer.decode(outputs[0], skip_special_tokens=True)

    # Remove the prompt from output
    command_text = generated_text.replace(prompt, "").strip()
    command_text = re.sub(r"```bash", "", command_text)
    command_text = re.sub(r"```", "", command_text)
    command_text = re.sub(r"<\|.*?\|>", "", command_text).strip()
    command_line = command_text.splitlines()[0].strip() if command_text else ""

    print(command_line)

if __name__ == "__main__":
    main()