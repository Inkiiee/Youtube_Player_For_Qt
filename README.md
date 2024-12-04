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

#Program Image
![스크린샷(177)](https://github.com/user-attachments/assets/160c2ebd-f403-4f8b-84be-692d4e970e53)
<main page>

![스크린샷(178)](https://github.com/user-attachments/assets/2f178627-f8fb-4b84-9aae-8b346028cfa0)
<search>

![스크린샷(179)](https://github.com/user-attachments/assets/94c99a6b-dc47-4c83-852b-ce0bdc8fbe0a)
<play with video window>

![스크린샷(180)](https://github.com/user-attachments/assets/4a9781ce-591b-4762-a4bd-470479ac3f2f)
<if closed video window>
