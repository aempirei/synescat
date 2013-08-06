module Main where

--
--
-- this is a haskell synthesizer
-- the output format is WAV 16-bit Unsigned Little-Endian
--
-- Copyright(c) 2008 Christopher Abad
-- aempirei@gmail.com
--
-- maybe you can get it to work and make an mp3 via lameenc
--
-- ghc synth.hs -o synth
-- ./synth < swinglow-hs.txt > swinglow.wav
-- lame -b160 test.wav test.mp3
--
--

import Data.Char

tempo = 135        :: Int -- quarter notes per minute
rate  = 22050      :: Int -- samples per second
spe   = round spe1 :: Int -- samples per 8th note

envelope = [0,1,0.8,0.8,0] :: [Double] -- envelope

spe1 = (itod rate) * 30.0 / (itod tempo)

itod a = fromIntegral a::Double

-- map key,octave to frequncy

frequency k o = 16.35 * (2.0 ** (o + (k / 12.0)))

-- i bet this can be done with an arrow

type Note = String
type Sound = (Int, Double)

duration x = spe * read (takeWhile (isDigit) x)
key      x = takeWhile (not . isDigit) (dropWhile (isDigit) x)
octave   x = read (dropWhile (not . isDigit) (dropWhile (isDigit) x)) :: Double

-- a sound is a tuple of (duration in samples, frequency in hz) 

sound x = (duration x, frequency (root (key x)) (octave x))      

sounds xs = map (sound) (words xs)

-- each musical key maps to an n/12th root of 2

root x = case (map toLower x) of
              "-"  -> -1 ; 
              "c"  -> 0  ;
              "c#" -> 1  ; "db" -> 1
              "d"  -> 2  ;
              "d#" -> 3  ; "eb" -> 3
              "e"  -> 4  ;
              "f"  -> 5  ;
              "f#" -> 6  ; "gb" -> 6
              "g"  -> 7  ;
              "g#" -> 8  ; "ab" -> 8
              "a"  -> 9  ;
              "a#" -> 10 ; "bb"  -> 10
              "b"  -> 11 ;

-- applyenvelope applys an envelope filter to a wave form
applyenvelope e w = zipWith (*) (etoc (length w) e) w

-- generate a ramp [0,1] of length n
ramp n = [x / itod (n - 1) | x <- [0..itod n - 1]] 

-- generate a convex combination [a,b] of length n
convex n a b = map (\x -> (b - a) * x + a) (ramp n)

-- convert Envelope to a convulution [Double] of length a
etoc _ [] = []
etoc _ (_:[]) = []
etoc a (x:y:ss) = (convex samples x y) ++ (etoc (a - samples) (y:ss))
                  where samples = a `div` length (y:ss)

-- genwav maps the sound tuples (duration,frequency) into actual PCM floating point data with range of [0,1]
genwav (a,x) = map (\y -> power (fromIntegral y::Double) x) [0..a-1]

-- these functions calculate the sinewave power over the time domain at time t with sinewave frequency of q

coef q = 2.0 * pi * q / (fromIntegral rate::Double)  -- coefficient for t to radians conversion
power t q = (sin (t * coef q) + 1.0) / 2.0           -- power of sinewave at t with frequency of q

-- remap floating point pcm to 16-bit signed pcm [0,1]::Double -> [-32768,32767]::Integer
-- this function is very fishy because it actually then remaps the negative values
-- to their unsigned twos-compliment equivalents 

pcm16 x = if z >= 0 then z else z + 65536
          where z = round (x * 65535 - 32768)

-- convert int to little endian of n bytes long (packed as String)

tole 0 _ = []
tole n a = chr (a `mod` 256) : (tole (n - 1) (a `div` 256))

le16 x = tole 2 x
le32 x = tole 4 x

-- convert music to wave information
mtow x = concat (map (applyenvelope envelope . genwav) (sounds x))

-- convert wave information to .wav packed into String
wtos x = concat (map (le16 . pcm16) x)

-- convert music to .wav
generateWav x = (wtos . mtow) x

-- generate a wave header (packed as String) given a wave file (packed as String)
generateHdr x = "RIFF" ++ le32 (36 + length x) ++
                "WAVEfmt " ++ le32 16 ++ le16 1 ++ le16 1 ++
                le32 rate ++ le32 (rate * 2) ++ le16 2 ++ le16 16 ++
                "data" ++ le32 (length x)

main = do x <- getContents
          stream <- return (generateWav x)
          header <- return (generateHdr stream)
          putStr (header ++ stream)
          return ()
