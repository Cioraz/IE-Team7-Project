import google.generativeai as genai

API_KEY = "AIzaSyBrmQLi-G6vuRp5jgPw6rEAoV8fj7YNCeU"
genai.configure(api_key=API_KEY)

model = genai.GenerativeModel('gemini-2.0-flash')

history = ""
print("Triggered1")

def chat_with_gemini(prompt):
    global history
    full_prompt = (
        "From now on, only respond with terminal commands. "
        "No explanations, descriptions, or extra words. Only commands, nothing else. "
        "Never include comments, explanations, or extra text. Just raw terminal commands.\n\n"
        f"{history}\nYou: {prompt}"
    )

    response = model.generate_content(full_prompt)
    history += f"\nYou: {prompt}\nGemini: {response.text}"
    print(response.text)
    return response.text

while True:
    user_input = input("\nYou: ")
    if user_input.lower() in ["exit", "quit"]:
        break
    chat_with_gemini(user_input)

