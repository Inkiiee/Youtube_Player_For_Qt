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

