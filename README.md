# Youtube_Player_For_Qt
A Qt program that interprets YouTube's signature and n-parameter to obtain the stream URL and plays it using QtMediaPlayer.

# Process

## 1. Parse video ID and player script from YouTube video page
Parse the video ID and extract the player script from the YouTube video page.

## 2. Extract decryption functions and SignatureTimeStamp (sts) from the player script
Extract the decryption function for the signature, the decryption function for the n-parameter, and the SignatureTimeStamp (sts) from the player script.

## 3. Retrieve video information using the Innertube API
Use the Innertube API to retrieve the required video information.

## 4. Decrypt `n` parameter and signature
Decrypt the `n` parameter and signature using the extracted functions.

## 5. Construct the video stream URL
Use the decrypted `n` parameter and signature to construct the video stream URL.

## 6. Download the video using the URL
Download the video content using the constructed stream URL.

## 7. Play the video using QMediaPlayer
Use QMediaPlayer to play the downloaded video.

# Program Image
![스크린샷(1)](https://github.com/user-attachments/assets/323f839a-07c3-46bc-aa35-d952125602a8)
<main page>

![스크린샷(1)](https://github.com/user-attachments/assets/0fd9844c-3ab2-4c02-9cc3-17c3c462811a)
<search>

![스크린샷(2)](https://github.com/user-attachments/assets/eb244d47-a33a-4df8-a57f-434db7c320d0)
![스크린샷(5)](https://github.com/user-attachments/assets/d23f7374-3bce-4bb0-92d8-507c3c34e2ed)
<play with video window>

![스크린샷(3)](https://github.com/user-attachments/assets/345c2c8f-7faf-4c74-829f-185fe2a6539f)
<if closed video window>

![스크린샷(4)](https://github.com/user-attachments/assets/1819f63e-f60e-4d50-aed0-dca6158207c2)

