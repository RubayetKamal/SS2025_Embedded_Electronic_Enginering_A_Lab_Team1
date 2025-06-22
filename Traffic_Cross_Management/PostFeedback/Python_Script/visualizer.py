#!/usr/bin/env python3
"""
Intersection Visualizer with Distinct Lane Paths
================================================
Features:
1. Separate paths for incoming and outgoing lanes
2. Cars stay in their designated lanes throughout
3. No center convergence - paths stay within lanes
4. Clockwise lane configuration maintained
"""
import pygame, sys, os, time, math
import numpy as np

# ───────── CONFIGURATION ───────────────────────────────────────────
W, H = 1100, 850
CX, CY = W // 2, H // 2
CAR_R = 10
BASE_SPEED = 200  # Pixels per second
TIME_SCALE = 0.25  # Slow motion factor

# Road dimensions
ROAD_WIDTH = 240
LANE_WIDTH = ROAD_WIDTH // 2  # Half for incoming, half for outgoing
APPROACH_DIST = 350
EXIT_DIST = 350
CURVE_RESOLUTION = 30  # Smoother curves

# Lane positions (clockwise from North to West)
LANE_CONFIG = {
    "NORTH": {"incoming": (0.5, 0), "outgoing": (-0.5, 0)},  # Factor for lane offset
    "EAST": {"incoming": (0, 0.5), "outgoing": (0, -0.5)},
    "SOUTH": {"incoming": (-0.5, 0), "outgoing": (0.5, 0)},
    "WEST": {"incoming": (0, -0.5), "outgoing": (0, 0.5)}
}

# ───────── DIRECTION CONSTANTS ─────────────────────────────────────
NORTH, SOUTH, EAST, WEST = 0, 1, 2, 3
DIR_NAMES = ["NORTH", "SOUTH", "EAST", "WEST"]
DIR_MAP = {"NORTH": NORTH, "SOUTH": SOUTH, "EAST": EAST, "WEST": WEST}
DIR_COLORS = [(255, 100, 100), (100, 255, 100), (100, 100, 255), (255, 255, 100)]

# Turn types
STRAIGHT, LEFT, RIGHT = 0, 1, 2
TURN_NAMES = ["STRAIGHT", "LEFT", "RIGHT"]
TURN_MAP = {"STRAIGHT": STRAIGHT, "LEFT": LEFT, "RIGHT": RIGHT}

# ───────── PYGAME INIT ─────────────────────────────────────────────
pygame.init()
screen = pygame.display.set_mode((W, H))
pygame.display.set_caption("Intersection Visualizer with Distinct Lane Paths")
clock = pygame.time.Clock()
font = pygame.font.SysFont("Arial", 18)
large_font = pygame.font.SysFont("Arial", 26)
small_font = pygame.font.SysFont("Arial", 14)


# ───────── ROAD DRAWING ────────────────────────────────────────────
def draw_road(s):
    s.fill((40, 40, 45))  # Dark background

    # Draw roads
    pygame.draw.rect(s, (60, 60, 70), (CX - ROAD_WIDTH // 2, 0, ROAD_WIDTH, H))
    pygame.draw.rect(s, (60, 60, 70), (0, CY - ROAD_WIDTH // 2, W, ROAD_WIDTH))

    # Draw lane dividers
    pygame.draw.line(s, (200, 200, 100), (CX, 0), (CX, H), 2)
    pygame.draw.line(s, (200, 200, 100), (0, CY), (W, CY), 2)

    # Draw lane boundaries
    pygame.draw.line(s, (180, 180, 100), (CX - LANE_WIDTH, 0), (CX - LANE_WIDTH, H), 2)
    pygame.draw.line(s, (180, 180, 100), (CX + LANE_WIDTH, 0), (CX + LANE_WIDTH, H), 2)
    pygame.draw.line(s, (180, 180, 100), (0, CY - LANE_WIDTH), (W, CY - LANE_WIDTH), 2)
    pygame.draw.line(s, (180, 180, 100), (0, CY + LANE_WIDTH), (W, CY + LANE_WIDTH), 2)

    # Draw center intersection marker
    pygame.draw.circle(s, (100, 200, 200), (CX, CY), 30, 3)
    center_text = small_font.render("CENTER", True, (200, 150, 150))
    s.blit(center_text, (CX - center_text.get_width() // 2, CY - 40))

    # Direction labels
    dir_text = [
        ("N", (CX, 30)),
        ("S", (CX, H - 30)),
        ("E", (W - 30, CY)),
        ("W", (30, CY))
    ]
    for text, pos in dir_text:
        text_surf = large_font.render(text, True, (200, 200, 100))
        s.blit(text_surf, (pos[0] - text_surf.get_width() // 2,
                           pos[1] - text_surf.get_height() // 2))

    # Lane labels
    lane_labels = [
        ("OUTGOING", (CX - LANE_WIDTH // 2, 70)),  # North outgoing
        ("INCOMING", (CX + LANE_WIDTH // 2, 70)),  # North incoming
        ("OUTGOING", (W - 70, CY - LANE_WIDTH // 2)),  # East outgoing
        ("INCOMING", (W - 70, CY + LANE_WIDTH // 2)),  # East incoming
        ("OUTGOING", (CX + LANE_WIDTH // 2, H - 70)),  # South outgoing
        ("INCOMING", (CX - LANE_WIDTH // 2, H - 70)),  # South incoming
        ("OUTGOING", (30, CY + LANE_WIDTH // 2)),  # West outgoing
        ("INCOMING", (30, CY - LANE_WIDTH // 2))  # West incoming
    ]
    for text, pos in lane_labels:
        label = small_font.render(text, True, (180, 180, 255))
        s.blit(label, pos)


# ───────── PATH GENERATION ─────────────────────────────────────────
def generate_curve_path(start, end, control, num_points=20):
    """Generate smooth bezier curve points"""
    points = []
    for t in np.linspace(0, 1, num_points):
        x = (1 - t) ** 2 * start[0] + 2 * (1 - t) * t * control[0] + t ** 2 * end[0]
        y = (1 - t) ** 2 * start[1] + 2 * (1 - t) * t * control[1] + t ** 2 * end[1]
        points.append((x, y))
    return points


def generate_path(entry_dir, turn_type):
    """Generate distinct path points that stay within lanes"""
    path = []
    entry_dir_name = DIR_NAMES[entry_dir]

    # Get lane offset factors
    incoming_offset = LANE_CONFIG[entry_dir_name]["incoming"]
    outgoing_offset = LANE_CONFIG[entry_dir_name]["outgoing"]

    # Calculate actual offsets
    in_offset_x = incoming_offset[0] * LANE_WIDTH
    in_offset_y = incoming_offset[1] * LANE_WIDTH
    out_offset_x = outgoing_offset[0] * LANE_WIDTH
    out_offset_y = outgoing_offset[1] * LANE_WIDTH

    # Entry points with lane positioning
    if entry_dir == NORTH:
        start = (CX + in_offset_x, -APPROACH_DIST)
        stop_line = (CX + in_offset_x, CY - ROAD_WIDTH // 2 - 10)
    elif entry_dir == SOUTH:
        start = (CX + in_offset_x, H + APPROACH_DIST)
        stop_line = (CX + in_offset_x, CY + ROAD_WIDTH // 2 + 10)
    elif entry_dir == EAST:
        start = (W + APPROACH_DIST, CY + in_offset_y)
        stop_line = (CX + ROAD_WIDTH // 2 + 10, CY + in_offset_y)
    else:  # WEST
        start = (-APPROACH_DIST, CY + in_offset_y)
        stop_line = (CX - ROAD_WIDTH // 2 - 10, CY + in_offset_y)

    # Determine exit direction
    if turn_type == STRAIGHT:
        if entry_dir == NORTH:
            exit_dir = SOUTH
        elif entry_dir == SOUTH:
            exit_dir = NORTH
        elif entry_dir == EAST:
            exit_dir = WEST
        else:
            exit_dir = EAST
    elif turn_type == LEFT:
        if entry_dir == NORTH:
            exit_dir = EAST
        elif entry_dir == SOUTH:
            exit_dir = WEST
        elif entry_dir == EAST:
            exit_dir = SOUTH
        else:
            exit_dir = NORTH
    else:  # RIGHT
        if entry_dir == NORTH:
            exit_dir = WEST
        elif entry_dir == SOUTH:
            exit_dir = EAST
        elif entry_dir == EAST:
            exit_dir = NORTH
        else:
            exit_dir = SOUTH

    exit_dir_name = DIR_NAMES[exit_dir]
    exit_out_offset = LANE_CONFIG[exit_dir_name]["outgoing"]
    exit_offset_x = exit_out_offset[0] * LANE_WIDTH
    exit_offset_y = exit_out_offset[1] * LANE_WIDTH

    # Exit points with lane positioning
    if exit_dir == NORTH:
        end = (CX + exit_offset_x, -EXIT_DIST)
        center_exit = (CX + exit_offset_x, CY - ROAD_WIDTH // 2)
    elif exit_dir == SOUTH:
        end = (CX + exit_offset_x, H + EXIT_DIST)
        center_exit = (CX + exit_offset_x, CY + ROAD_WIDTH // 2)
    elif exit_dir == EAST:
        end = (W + EXIT_DIST, CY + exit_offset_y)
        center_exit = (CX + ROAD_WIDTH // 2, CY + exit_offset_y)
    else:  # WEST
        end = (-EXIT_DIST, CY + exit_offset_y)
        center_exit = (CX - ROAD_WIDTH // 2, CY + exit_offset_y)

    # Build path with lane-specific curves
    path.append(start)
    path.append(stop_line)

    if turn_type == STRAIGHT:
        # Direct path staying in lane
        path.append(center_exit)
    else:
        # Curve control points adjusted for lane position
        if entry_dir == NORTH:
            if turn_type == LEFT:
                control = (CX + in_offset_x + LANE_WIDTH, CY - LANE_WIDTH)
            else:  # RIGHT
                control = (CX + in_offset_x - LANE_WIDTH, CY - LANE_WIDTH)
        elif entry_dir == SOUTH:
            if turn_type == LEFT:
                control = (CX + in_offset_x - LANE_WIDTH, CY + LANE_WIDTH)
            else:  # RIGHT
                control = (CX + in_offset_x + LANE_WIDTH, CY + LANE_WIDTH)
        elif entry_dir == EAST:
            if turn_type == LEFT:
                control = (CX + LANE_WIDTH, CY + in_offset_y + LANE_WIDTH)
            else:  # RIGHT
                control = (CX + LANE_WIDTH, CY + in_offset_y - LANE_WIDTH)
        else:  # WEST
            if turn_type == LEFT:
                control = (CX - LANE_WIDTH, CY + in_offset_y - LANE_WIDTH)
            else:  # RIGHT
                control = (CX - LANE_WIDTH, CY + in_offset_y + LANE_WIDTH)

        # Generate smooth curve staying within lane
        curve_points = generate_curve_path(stop_line, center_exit, control, CURVE_RESOLUTION)
        path.extend(curve_points)

    path.append(center_exit)
    path.append(end)

    return path, exit_dir


# ───────── CAR CLASS ───────────────────────────────────────────────
class Car:
    def __init__(self, car_id, entry_dir, turn_type):
        self.car_id = car_id
        self.entry_dir = entry_dir
        self.turn_type = turn_type
        self.path, self.exit_dir = generate_path(entry_dir, turn_type)
        self.current_segment = 0
        self.x, self.y = self.path[0]
        self.wait = True
        self.color = DIR_COLORS[entry_dir]
        self.speed = BASE_SPEED
        self.last_update = time.time()
        self.position_history = []
        self.state = "APPROACHING"
        self.turn_signal = 0
        print(f"Created car {car_id} from {DIR_NAMES[entry_dir]} turning {TURN_NAMES[turn_type]}")

    def update(self, dt):
        """Move car along path"""
        dt_scaled = dt * TIME_SCALE

        start_x, start_y = self.path[self.current_segment]
        end_x, end_y = self.path[self.current_segment + 1]

        dx = end_x - start_x
        dy = end_y - start_y
        distance = max(math.sqrt(dx * dx + dy * dy), 0.001)

        move_dist = min(self.speed * dt_scaled, distance)
        seg_progress = move_dist / distance

        self.x += dx * seg_progress
        self.y += dy * seg_progress

        if len(self.position_history) < 50:
            self.position_history.append((self.x, self.y))

        self.turn_signal = (self.turn_signal + dt_scaled * 5) % 2

        if math.hypot(self.x - end_x, self.y - end_y) < 5.0:
            self.current_segment += 1
            if self.current_segment < len(self.path) - 1:
                if self.current_segment == 1:
                    self.state = "WAITING"
                elif self.current_segment == 2 and self.turn_type != STRAIGHT:
                    self.state = "TURNING"
                elif self.current_segment == len(self.path) - 2:
                    self.state = "EXITING"
                return True
            else:
                self.state = "EXITED"
                print(f"Car {self.car_id} exited normally")
                return False
        return True

    def draw(self, s, is_tracked=False):
        # Draw trail
        for i, pos in enumerate(self.position_history):
            alpha = min(200, i * 5)
            radius = max(1, int(2 * i / len(self.position_history)))
            pygame.draw.circle(s, (*self.color, alpha), (int(pos[0]), int(pos[1])), radius)

        # Draw car
        car_color = self.color
        border_color = (255, 255, 255)
        border_width = 2

        if is_tracked:
            border_color = (255, 255, 0)
            border_width = 3
            id_text = font.render(str(self.car_id), True, (255, 255, 0))
            s.blit(id_text, (self.x - id_text.get_width() // 2, self.y - 35))
        else:
            id_text = small_font.render(str(self.car_id), True, (200, 200, 200))
            s.blit(id_text, (self.x - id_text.get_width() // 2, self.y - 25))

        # Turn signal
        if self.turn_signal < 1.0 and self.state != "APPROACHING":
            signal_color = (255, 200, 0)
            if self.turn_type == LEFT:
                signal_pos = (self.x - CAR_R * 1.5, self.y)
            else:
                signal_pos = (self.x + CAR_R * 1.5, self.y)
            pygame.draw.circle(s, signal_color, (int(signal_pos[0]), int(signal_pos[1])), CAR_R // 2)

        pygame.draw.circle(s, car_color, (int(self.x), int(self.y)), CAR_R)
        pygame.draw.circle(s, border_color, (int(self.x), int(self.y)), CAR_R, border_width)

        # Direction indicator
        if self.current_segment < len(self.path) - 1:
            angle = math.atan2(self.path[self.current_segment + 1][1] - self.y,
                               self.path[self.current_segment + 1][0] - self.x)
            end_x = self.x + math.cos(angle) * (CAR_R + 6)
            end_y = self.y + math.sin(angle) * (CAR_R + 6)
            pygame.draw.line(s, (255, 255, 255), (self.x, self.y), (end_x, end_y), 2)

        if is_tracked:
            state_text = small_font.render(self.state, True, (220, 220, 220))
            s.blit(state_text, (self.x - state_text.get_width() // 2, self.y + 20))

            # Draw lane info
            lane_info = small_font.render(
                f"Lane: {DIR_NAMES[self.entry_dir]} {['OUTGOING', 'INCOMING'][self.entry_dir % 2]}",
                True, (200, 255, 200))
            s.blit(lane_info, (self.x - lane_info.get_width() // 2, self.y + 40))


# ───────── LOG READER ──────────────────────────────────────────────
class LogMonitor:
    def __init__(self, file_path):
        self.file_path = file_path
        self.last_pos = 0
        if not os.path.exists(file_path):
            open(file_path, 'w').close()
            print(f"Created log file at {file_path}")
        else:
            self.last_pos = os.path.getsize(file_path)

    def get_new_events(self):
        try:
            with open(self.file_path, 'r') as f:
                f.seek(self.last_pos)
                new_content = f.read()
                self.last_pos = f.tell()
                return [line.strip() for line in new_content.splitlines() if line.strip()]
        except Exception as e:
            print(f"Log error: {str(e)}")
            return []


# ───────── EVENT PROCESSOR ────────────────────────────────────────
class EventProcessor:
    def __init__(self):
        self.cars = {}
        self.active_cars = set()
        self.removed_cars = set()

    def process_events(self, log_events):
        for line in log_events:
            parts = [p.strip() for p in line.split(',')]
            if len(parts) < 5:
                continue

            event_type = parts[1].upper()
            car_id = parts[2]
            entry_dir_str = parts[3].upper()
            turn_type_str = parts[4].upper()

            entry_dir = DIR_MAP.get(entry_dir_str, NORTH)
            turn_type = TURN_MAP.get(turn_type_str, STRAIGHT)

            if event_type == "ARRIVAL":
                if car_id not in self.cars:
                    self.cars[car_id] = Car(car_id, entry_dir, turn_type)
                    self.active_cars.add(car_id)
                    print(f"Created car {car_id} from {entry_dir_str} turning {turn_type_str}")

            elif event_type == "ENTER":
                if car_id in self.cars:
                    self.cars[car_id].wait = False
                    self.cars[car_id].state = "MOVING"

            elif event_type == "EXIT":
                if car_id in self.cars:
                    self.cars[car_id].state = "EXITING"
                    self.removed_cars.add(car_id)

    def update_cars(self, dt):
        cars_to_remove = []

        for car_id in list(self.active_cars):
            car = self.cars[car_id]
            if not car.update(dt):
                cars_to_remove.append(car_id)

        for car_id in cars_to_remove:
            if car_id in self.active_cars:
                self.active_cars.remove(car_id)
                if car_id in self.removed_cars:
                    self.removed_cars.remove(car_id)
                print(f"Removed car {car_id}")

        for car_id in list(self.removed_cars):
            if car_id in self.active_cars:
                self.active_cars.remove(car_id)
                self.removed_cars.remove(car_id)
                print(f"Force removed car {car_id}")


# ───────── MAIN LOOP ──────────────────────────────────────────────
def main():
    log_monitor = LogMonitor(LOG_FILE)
    event_processor = EventProcessor()
    frame_count = 0
    last_time = time.time()
    paused = False
    sim_speed = 1.0

    # Debug panel text
    debug_text = [
        "CONTROLS:",
        "SPACE - Pause/Resume simulation",
        "UP/DOWN - Adjust simulation speed",
        "R - Reset simulation",
        f"Tracking: Car {TRACK_ID}",
        "",
        "LANE CONFIGURATION:",
        "Clockwise from North to West:",
        "1. North: OUTGOING (top), INCOMING (bottom)",
        "2. East: OUTGOING (right), INCOMING (left)",
        "3. South: OUTGOING (bottom), INCOMING (top)",
        "4. West: OUTGOING (left), INCOMING (right)"
    ]

    print(f"Tracking car ID: {TRACK_ID}")
    print(f"Monitoring log: {LOG_FILE}")

    while True:
        current_time = time.time()
        dt = current_time - last_time
        last_time = current_time
        frame_count += 1

        for ev in pygame.event.get():
            if ev.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif ev.type == pygame.KEYDOWN:
                if ev.key == pygame.K_SPACE:
                    paused = not paused
                    print(f"Simulation {'PAUSED' if paused else 'RESUMED'}")
                elif ev.key == pygame.K_r:
                    event_processor = EventProcessor()
                    print("Simulation reset")
                elif ev.key == pygame.K_UP:
                    sim_speed = min(sim_speed + 0.1, 3.0)
                    print(f"Speed increased to {sim_speed:.1f}x")
                elif ev.key == pygame.K_DOWN:
                    sim_speed = max(sim_speed - 0.1, 0.1)
                    print(f"Speed decreased to {sim_speed:.1f}x")

        if not paused:
            new_log_events = log_monitor.get_new_events()
            if new_log_events:
                event_processor.process_events(new_log_events)

        if not paused:
            event_processor.update_cars(dt * sim_speed)

        draw_road(screen)

        for car_id in event_processor.active_cars:
            is_tracked = (car_id == TRACK_ID)
            event_processor.cars[car_id].draw(screen, is_tracked)

        # Draw info panel
        pygame.draw.rect(screen, (30, 30, 50, 220), (10, 10, 350, 200))
        pygame.draw.rect(screen, (30, 30, 50, 220), (W - 350, 10, 340, 200))

        status_info = [
            f"Active Cars: {len(event_processor.active_cars)}",
            f"Sim Speed: {sim_speed:.1f}x",
            f"Paused: {'Yes' if paused else 'No'}",
            f"Tracked Car: {TRACK_ID}",
            f"Road Width: {ROAD_WIDTH}px",
            f"Lane Width: {LANE_WIDTH}px"
        ]

        for i, text in enumerate(status_info):
            text_surf = font.render(text, True, (200, 220, 255))
            screen.blit(text_surf, (20, 15 + i * 25))

        for i, text in enumerate(debug_text):
            ctrl_text = small_font.render(text, True, (180, 220, 255))
            screen.blit(ctrl_text, (W - 340, 15 + i * 20))

        if TRACK_ID in event_processor.cars and TRACK_ID in event_processor.active_cars:
            car = event_processor.cars[TRACK_ID]
            track_info = [
                f"Tracked Car {TRACK_ID}:",
                f"State: {car.state}",
                f"From: {DIR_NAMES[car.entry_dir]}",
                f"Turn: {TURN_NAMES[car.turn_type]}",
                f"To: {DIR_NAMES[car.exit_dir]}"
            ]

            pygame.draw.rect(screen, (50, 50, 70, 220), (W - 350, H - 150, 340, 140))
            for i, text in enumerate(track_info):
                text_surf = font.render(text, True, (255, 255, 100))
                screen.blit(text_surf, (W - 340, H - 140 + i * 25))

        if paused:
            pause_text = large_font.render("PAUSED", True, (255, 100, 100))
            screen.blit(pause_text, (W // 2 - pause_text.get_width() // 2, 30))

        pygame.display.flip()
        clock.tick(60)


if __name__ == "__main__":
    LOG_FILE = r"C:\Users\User\PyCharmMiscProject\intersection_log.txt"
    TRACK_ID = '1'

    main()