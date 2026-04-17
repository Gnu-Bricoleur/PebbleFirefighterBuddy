<?php

header('Content-Type: application/json');

// ---- SECURITY ----
if (!isset($_GET['token']) || $_GET['token'] !== 'SECRET123') {
    echo json_encode(["found" => false]);
    exit;
}

// ---- CONFIG ----
$hostname = '{imap.gmail.com:993/imap/ssl}INBOX';
$username = 'your@email.com';
$password = 'your_app_password';

// ---- CONNECT ----
$inbox = imap_open($hostname, $username, $password);

if (!$inbox) {
    echo json_encode(["found" => false]);
    exit;
}

$date = date("d-M-Y");
$emails = imap_search($inbox, 'SINCE "' . $date . '"');

$found = false;
$text = "";

if ($emails) {
    rsort($emails);

    foreach ($emails as $email_number) {
        $overview = imap_fetch_overview($inbox, $email_number, 0)[0];

        $subject = strtolower($overview->subject ?? '');
        $timestamp = strtotime($overview->date);

        if (time() - $timestamp <= 300) {
            if (strpos($subject, 'info mobilisation') !== false) {

                $body = imap_fetchbody($inbox, $email_number, 1);

                $body = trim(strip_tags($body));
                $body = preg_replace('/\s+/', ' ', $body);

                $text = substr($body, 0, 100);

                $found = true;
                break;
            }
        }
    }
}

imap_close($inbox);

echo json_encode([
    "found" => $found,
    "text" => $text
]);