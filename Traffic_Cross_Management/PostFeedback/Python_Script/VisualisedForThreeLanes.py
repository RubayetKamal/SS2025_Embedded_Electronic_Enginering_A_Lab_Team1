#!/usr/bin/env python3
"""
German-Style Intersection Visualizer - Final Corrected Version
============================================================
Key Fixes:
1. Corrected exit paths to proper incoming lanes
2. Added collision detection
3. Fixed lane positioning for all roads
"""
import pygame
import sys
import os
import time
import math
import numpy as np

# Configuration
WIDTH, HEIGHT = 1100, 850
CENTER_X, CENTER_Y = WIDTH // 2, HEIGHT // 2
ROAD_WIDTH = 300
LANE_WIDTH = ROAD_WIDTH // 3
HALF_ROAD = ROAD_WIDTH // 2
CAR_RADIUS = 10
BASE_SPEED = 200  # pixels per second
TIME_SCALE = 0.25  # slow motion factor
SAFE_DISTANCE = 25  # collision distance
APPROACH_DIST = 350
EXIT_DIST = 350
CURVE_RESOLUTION = 30
LOG_FILE = "intersection_log.txt"
TRACKED_CAR_ID = "1"

# Colors
BACKGROUND = (40, 40, 45)
ROAD_COLOR = (60, 60, 70)
LANE_MARKER = (200, 200, 100)
INCOMING_COLOR = (0, 0, 255)     # Blue
OUTGOING_COLOR = (255, 0, 0)     # Red
RIGHT_TURN_COLOR = (0, 255, 0)   # Green
CENTER_COLOR = (100, 200, 200)
TEXT_COLOR = (200, 200, 200)
HIGHLIGHT_COLOR = (255, 255, 0)
COLLISION_COLOR = (255, 0, 255)  # Purple for collisions

# Directions
NORTH, SOUTH, EAST, WEST = 0, 1, 2, 3
DIR_NAMES = ["NORTH", "SOUTH", "EAST", "WEST"]
DIR_COLORS = [(255, 100, 100), (100, 255, 100), (100, 100, 255), (255, 255, 100)]
TURN_NAMES = ["STRAIGHT", "LEFT", "RIGHT"]

# Initialize pygame
pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("German-Style Intersection - Final Corrected")
clock = pygame.time.Clock()
font = pygame.font.SysFont("Arial", 18)
large_font = pygame.font.SysFont("Arial", 24)
small_font = pygame.font.SysFont("Arial", 14)

class Car:
    def __init__(self, car_id, entry_dir, turn_type):
        self.id = car_id
        self.entry_dir = entry_dir
        self.turn_type = turn_type
        self.color = DIR_COLORS[entry_dir]
        self.x, self.y = self.get_start_position()
        self.path = []
        self.current_target = 0
        self.speed = BASE_SPEED
        self.position_history = []
        self.state = "APPROACHING"
        self.waiting = False
        self.wait_start = 0
        self.generate_path()
        print(f"Created car {car_id} from {DIR_NAMES[entry_dir]} turning {TURN_NAMES[turn_type]}")
        
    def get_start_position(self):
        """Get starting position with correct lane positioning"""
        if self.entry_dir == NORTH:
            # Right lane on right side (east)
            lane_offset = LANE_WIDTH if self.turn_type == 2 else 0
            return CENTER_X + lane_offset, -APPROACH_DIST
        elif self.entry_dir == SOUTH:
            # Right lane on right side (east)
            lane_offset = LANE_WIDTH if self.turn_type == 2 else 0
            return CENTER_X + lane_offset, HEIGHT + APPROACH_DIST
        elif self.entry_dir == EAST:
            # Right lane on bottom side (south)
            lane_offset = LANE_WIDTH if self.turn_type == 2 else 0
            return WIDTH + APPROACH_DIST, CENTER_Y + lane_offset
        else:  # WEST
            # CORRECTED: Right lane on top side (north)
            # For WEST, +y is down, so -LANE_WIDTH is up (north)
            lane_offset = -LANE_WIDTH if self.turn_type == 2 else 0
            return -APPROACH_DIST, CENTER_Y + lane_offset

    def generate_path(self):
        """Generate path based on turn type"""
        # Stop line position
        if self.entry_dir == NORTH:
            self.path.append((self.x, CENTER_Y - HALF_ROAD - 20))
        elif self.entry_dir == SOUTH:
            self.path.append((self.x, CENTER_Y + HALF_ROAD + 20))
        elif self.entry_dir == EAST:
            self.path.append((CENTER_X + HALF_ROAD + 20, self.y))
        else:  # WEST
            self.path.append((CENTER_X - HALF_ROAD - 20, self.y))
        
        # Determine exit direction
        if self.turn_type == 0:  # Straight
            exit_dir = (self.entry_dir + 2) % 4
        elif self.turn_type == 1:  # Left
            exit_dir = (self.entry_dir + 1) % 4
        else:  # Right
            exit_dir = (self.entry_dir - 1) % 4
        
        # Control points for curves
        control = (CENTER_X, CENTER_Y)
        
        # EXIT TO INCOMING LANE (LEFT LANE) OF TARGET ROAD
        if exit_dir == NORTH:
            end = (CENTER_X - LANE_WIDTH, -EXIT_DIST)  # NL
            center_exit = (CENTER_X - LANE_WIDTH, CENTER_Y - HALF_ROAD)
        elif exit_dir == SOUTH:
            end = (CENTER_X - LANE_WIDTH, HEIGHT + EXIT_DIST)  # SL
            center_exit = (CENTER_X - LANE_WIDTH, CENTER_Y + HALF_ROAD)
        elif exit_dir == EAST:
            end = (WIDTH + EXIT_DIST, CENTER_Y + LANE_WIDTH)  # EL
            center_exit = (CENTER_X + HALF_ROAD, CENTER_Y + LANE_WIDTH)
        else:  # WEST
            end = (-EXIT_DIST, CENTER_Y + LANE_WIDTH)  # WL
            center_exit = (CENTER_X - HALF_ROAD, CENTER_Y + LANE_WIDTH)
        
        # Add curve points for turns
        if self.turn_type != 0:  # Not straight
            # Generate curve points
            start_x, start_y = self.path[-1]
            curve_points = []
            for t in np.linspace(0, 1, CURVE_RESOLUTION):
                # Quadratic bezier curve
                x = (1-t)**2 * start_x + 2*(1-t)*t * control[0] + t**2 * center_exit[0]
                y = (1-t)**2 * start_y + 2*(1-t)*t * control[1] + t**2 * center_exit[1]
                curve_points.append((x, y))
            self.path.extend(curve_points)
        
        self.path.append(center_exit)
        self.path.append(end)

    def check_collision(self, other_car):
        """Check if this car is colliding with another car"""
        distance = math.sqrt((self.x - other_car.x)**2 + (self.y - other_car.y)**2)
        return distance < SAFE_DISTANCE

    def update(self, dt, all_cars):
        """Move car along path with timing and waiting"""
        dt_scaled = dt * TIME_SCALE
        
        if self.current_target >= len(self.path):
            self.state = "EXITED"
            return False
            
        if self.state == "WAITING":
            # Check if intersection is clear before moving
            intersection_clear = True
            for car_id, other in all_cars.items():
                if other is self or other.state == "EXITED":
                    continue
                    
                # Check if other car is in the intersection area
                if (abs(other.x - CENTER_X) < HALF_ROAD + 50 and 
                    abs(other.y - CENTER_Y) < HALF_ROAD + 50):
                    intersection_clear = False
                    break
                    
            if intersection_clear:
                self.state = "MOVING"
            return True
            
        # Check for collisions before moving
        for car_id, other in all_cars.items():
            if other is not self and other.state != "EXITED":
                if self.check_collision(other):
                    return True  # Don't move if collision detected
                    
        target_x, target_y = self.path[self.current_target]
        dx = target_x - self.x
        dy = target_y - self.y
        distance = math.sqrt(dx*dx + dy*dy)
        
        if distance < 5:
            self.current_target += 1
            if self.current_target == 1:  # Reached stop line
                self.state = "WAITING"
                self.waiting = True
                self.wait_start = time.time()
            return True
            
        # Move toward target
        move_dist = min(self.speed * dt_scaled, distance)
        self.x += dx * move_dist / distance
        self.y += dy * move_dist / distance
        
        # Add to position history
        if len(self.position_history) < 20:
            self.position_history.append((self.x, self.y))
            
        return True

    def draw(self, surface, is_tracked=False):
        """Draw car with trail and info"""
        # Draw trail
        for i, pos in enumerate(self.position_history):
            alpha = min(200, i * 10)
            radius = max(1, CAR_RADIUS * i // len(self.position_history))
            pygame.draw.circle(surface, (*self.color, alpha), (int(pos[0]), int(pos[1])), radius)
        
        # Draw car
        border_color = HIGHLIGHT_COLOR if is_tracked else (255, 255, 255)
        pygame.draw.circle(surface, self.color, (int(self.x), int(self.y)), CAR_RADIUS)
        pygame.draw.circle(surface, border_color, (int(self.x), int(self.y)), CAR_RADIUS, 2)
        
        # Draw car ID
        id_text = font.render(str(self.id), True, border_color)
        surface.blit(id_text, (self.x - id_text.get_width()//2, self.y - 25))
        
        # Draw state info if tracked
        if is_tracked:
            state_text = small_font.render(self.state, True, (220, 220, 220))
            surface.blit(state_text, (self.x - state_text.get_width()//2, self.y + 20))
            
            turn_text = small_font.render(f"Turn: {TURN_NAMES[self.turn_type]}", True, (200, 255, 200))
            surface.blit(turn_text, (self.x - turn_text.get_width()//2, self.y + 40))

def draw_roads(surface):
    """Draw the intersection with correct lane positioning"""
    surface.fill(BACKGROUND)
    
    # Draw roads
    pygame.draw.rect(surface, ROAD_COLOR, 
                    (CENTER_X - HALF_ROAD, 0, ROAD_WIDTH, HEIGHT))
    pygame.draw.rect(surface, ROAD_COLOR, 
                    (0, CENTER_Y - HALF_ROAD, WIDTH, ROAD_WIDTH))
    
    # Draw lane markers
    for offset in [-LANE_WIDTH, 0, LANE_WIDTH]:
        pygame.draw.line(surface, LANE_MARKER, 
                         (CENTER_X + offset, 0), 
                         (CENTER_X + offset, HEIGHT), 2)
        pygame.draw.line(surface, LANE_MARKER, 
                         (0, CENTER_Y + offset), 
                         (WIDTH, CENTER_Y + offset), 2)
    
    # Draw center circle
    pygame.draw.circle(surface, CENTER_COLOR, (CENTER_X, CENTER_Y), 30, 2)
    center_text = small_font.render("CENTER", True, (200, 150, 150))
    surface.blit(center_text, (CENTER_X - center_text.get_width()//2, CENTER_Y - 40))
    
    # Draw lane labels - NORTH ROAD (TOP)
    pygame.draw.line(surface, INCOMING_COLOR, 
                    (CENTER_X - LANE_WIDTH, 40), 
                    (CENTER_X - LANE_WIDTH, 80), 3)
    pygame.draw.line(surface, OUTGOING_COLOR, 
                    (CENTER_X, 40), 
                    (CENTER_X, 80), 3)
    pygame.draw.line(surface, RIGHT_TURN_COLOR, 
                    (CENTER_X + LANE_WIDTH, 40), 
                    (CENTER_X + LANE_WIDTH, 80), 3)
    
    # Draw text labels
    nl_text = small_font.render("NL (INCOMING)", True, INCOMING_COLOR)
    nm_text = small_font.render("NM (OUTGOING)", True, OUTGOING_COLOR)
    nr_text = small_font.render("NR (RIGHT)", True, RIGHT_TURN_COLOR)
    surface.blit(nl_text, (CENTER_X - LANE_WIDTH - nl_text.get_width()//2, 90))
    surface.blit(nm_text, (CENTER_X - nm_text.get_width()//2, 90))
    surface.blit(nr_text, (CENTER_X + LANE_WIDTH - nr_text.get_width()//2, 90))
    
    # WEST ROAD (LEFT) - Corrected labels
    pygame.draw.line(surface, RIGHT_TURN_COLOR, (40, CENTER_Y - LANE_WIDTH), (80, CENTER_Y - LANE_WIDTH), 3)  # WR
    pygame.draw.line(surface, OUTGOING_COLOR, (40, CENTER_Y), (80, CENTER_Y), 3)  # WM
    pygame.draw.line(surface, INCOMING_COLOR, (40, CENTER_Y + LANE_WIDTH), (80, CENTER_Y + LANE_WIDTH), 3)  # WL
    
    wr_text = small_font.render("WR (RIGHT)", True, RIGHT_TURN_COLOR)
    wm_text = small_font.render("WM (OUTGOING)", True, OUTGOING_COLOR)
    wl_text = small_font.render("WL (INCOMING)", True, INCOMING_COLOR)
    surface.blit(wr_text, (90, CENTER_Y - LANE_WIDTH - wr_text.get_height()//2))
    surface.blit(wm_text, (90, CENTER_Y - wm_text.get_height()//2))
    surface.blit(wl_text, (90, CENTER_Y + LANE_WIDTH - wl_text.get_height()//2))
    
    # EAST ROAD (RIGHT) - Corrected labels
    pygame.draw.line(surface, INCOMING_COLOR, (WIDTH-80, CENTER_Y + LANE_WIDTH), (WIDTH-40, CENTER_Y + LANE_WIDTH), 3)  # EL
    pygame.draw.line(surface, OUTGOING_COLOR, (WIDTH-80, CENTER_Y), (WIDTH-40, CENTER_Y), 3)  # EM
    pygame.draw.line(surface, RIGHT_TURN_COLOR, (WIDTH-80, CENTER_Y - LANE_WIDTH), (WIDTH-40, CENTER_Y - LANE_WIDTH), 3)  # ER
    
    el_text = small_font.render("EL (INCOMING)", True, INCOMING_COLOR)
    em_text = small_font.render("EM (OUTGOING)", True, OUTGOING_COLOR)
    er_text = small_font.render("ER (RIGHT)", True, RIGHT_TURN_COLOR)
    surface.blit(el_text, (WIDTH-150, CENTER_Y + LANE_WIDTH - el_text.get_height()//2))
    surface.blit(em_text, (WIDTH-150, CENTER_Y - em_text.get_height()//2))
    surface.blit(er_text, (WIDTH-150, CENTER_Y - LANE_WIDTH - er_text.get_height()//2))
    
    # SOUTH ROAD (BOTTOM) - Corrected labels
    pygame.draw.line(surface, INCOMING_COLOR, (CENTER_X - LANE_WIDTH, HEIGHT-80), (CENTER_X - LANE_WIDTH, HEIGHT-40), 3)  # SL
    pygame.draw.line(surface, OUTGOING_COLOR, (CENTER_X, HEIGHT-80), (CENTER_X, HEIGHT-40), 3)  # SM
    pygame.draw.line(surface, RIGHT_TURN_COLOR, (CENTER_X + LANE_WIDTH, HEIGHT-80), (CENTER_X + LANE_WIDTH, HEIGHT-40), 3)  # SR
    
    sl_text = small_font.render("SL (INCOMING)", True, INCOMING_COLOR)
    sm_text = small_font.render("SM (OUTGOING)", True, OUTGOING_COLOR)
    sr_text = small_font.render("SR (RIGHT)", True, RIGHT_TURN_COLOR)
    surface.blit(sl_text, (CENTER_X - LANE_WIDTH - sl_text.get_width()//2, HEIGHT-90))
    surface.blit(sm_text, (CENTER_X - sm_text.get_width()//2, HEIGHT-90))
    surface.blit(sr_text, (CENTER_X + LANE_WIDTH - sr_text.get_width()//2, HEIGHT-90))
    
    # Draw direction labels
    n_text = large_font.render("N", True, (200, 200, 100))
    s_text = large_font.render("S", True, (200, 200, 100))
    e_text = large_font.render("E", True, (200, 200, 100))
    w_text = large_font.render("W", True, (200, 200, 100))
    surface.blit(n_text, (CENTER_X - n_text.get_width()//2, 10))
    surface.blit(s_text, (CENTER_X - s_text.get_width()//2, HEIGHT - 40))
    surface.blit(e_text, (WIDTH - 40, CENTER_Y - e_text.get_height()//2))
    surface.blit(w_text, (10, CENTER_Y - w_text.get_height()//2))

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

class Simulation:
    def __init__(self):
        self.cars = {}
        self.active_cars = {}
        self.car_counter = 1
        self.tracked_car = None
        self.paused = False
        self.sim_speed = 1.0
        
    def process_event(self, event_str):
        parts = [p.strip() for p in event_str.split(',')]
        if len(parts) < 5:
            return
            
        event_type = parts[1].upper()
        car_id = parts[2]
        entry_dir_str = parts[3].upper()
        turn_type_str = parts[4].upper()
        
        # Map direction
        if entry_dir_str == "NORTH":
            entry_dir = NORTH
        elif entry_dir_str == "SOUTH":
            entry_dir = SOUTH
        elif entry_dir_str == "EAST":
            entry_dir = EAST
        elif entry_dir_str == "WEST":
            entry_dir = WEST
        else:
            return
            
        # Map turn type
        if turn_type_str == "STRAIGHT":
            turn_type = 0
        elif turn_type_str == "LEFT":
            turn_type = 1
        elif turn_type_str == "RIGHT":
            turn_type = 2
        else:
            return
            
        if event_type == "ARRIVAL":
            if car_id not in self.active_cars:
                self.active_cars[car_id] = Car(car_id, entry_dir, turn_type)
                print(f"Car {car_id} arrived from {entry_dir_str} turning {turn_type_str}")
                
        elif event_type == "ENTER":
            if car_id in self.active_cars:
                car = self.active_cars[car_id]
                if car.state == "WAITING":
                    car.state = "MOVING"
                    print(f"Car {car_id} entered intersection")
                    
        elif event_type == "EXIT":
            if car_id in self.active_cars:
                print(f"Car {car_id} exited intersection")
                del self.active_cars[car_id]
                if self.tracked_car == car_id:
                    self.tracked_car = None

    def update(self, dt):
        if self.paused:
            return
            
        cars_to_remove = []
        for car_id, car in list(self.active_cars.items()):
            if not car.update(dt * self.sim_speed, self.active_cars):
                cars_to_remove.append(car_id)
                
        for car_id in cars_to_remove:
            del self.active_cars[car_id]
            if self.tracked_car == car_id:
                self.tracked_car = None

    def draw(self, surface):
        draw_roads(surface)
        
        # Draw collision indicators first
        for car_id1, car1 in self.active_cars.items():
            for car_id2, car2 in self.active_cars.items():
                if car_id1 < car_id2 and car1.check_collision(car2):
                    mid_x = (car1.x + car2.x) / 2
                    mid_y = (car1.y + car2.y) / 2
                    pygame.draw.circle(surface, COLLISION_COLOR, (int(mid_x), int(mid_y)), 15)
                    collision_text = font.render("COLLISION", True, (255, 255, 255))
                    surface.blit(collision_text, (mid_x - collision_text.get_width()//2, mid_y - 20))
        
        # Then draw cars
        for car_id, car in self.active_cars.items():
            car.draw(surface, is_tracked=(car_id == self.tracked_car))
            
        # Draw info panel
        pygame.draw.rect(surface, (30, 30, 50, 180), (10, 10, 350, 180))
        info = [
            f"Active Cars: {len(self.active_cars)}",
            f"Tracked Car: {self.tracked_car if self.tracked_car else 'None'}",
            f"Sim Speed: {self.sim_speed:.1f}x",
            f"Paused: {'Yes' if self.paused else 'No'}",
            f"Log: {LOG_FILE}",
            "",
            "CONTROLS:",
            "SPACE: Pause/Resume",
            "UP/DOWN: Adjust speed",
            "R: Reset simulation",
            "T: Track next car"
        ]
        
        for i, text in enumerate(info):
            text_surf = small_font.render(text, True, TEXT_COLOR)
            surface.blit(text_surf, (20, 15 + i*20))
            
        # Draw title
        title = large_font.render("German-Style Intersection Simulator", True, (255, 255, 200))
        surface.blit(title, (WIDTH//2 - title.get_width()//2, 10))
        
        # Draw lane info
        lane_info = font.render("Cars exit to incoming lanes (NL, SL, EL, WL)", True, (200, 255, 200))
        surface.blit(lane_info, (WIDTH//2 - lane_info.get_width()//2, HEIGHT - 30))

def main():
    sim = Simulation()
    log_monitor = LogMonitor(LOG_FILE)
    last_time = time.time()
    
    running = True
    while running:
        current_time = time.time()
        dt = current_time - last_time
        last_time = current_time
        
        # Handle events
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_SPACE:
                    sim.paused = not sim.paused
                elif event.key == pygame.K_UP:
                    sim.sim_speed = min(sim.sim_speed + 0.1, 3.0)
                elif event.key == pygame.K_DOWN:
                    sim.sim_speed = max(sim.sim_speed - 0.1, 0.1)
                elif event.key == pygame.K_r:
                    sim = Simulation()
                    log_monitor = LogMonitor(LOG_FILE)
                elif event.key == pygame.K_t:
                    if sim.active_cars:
                        car_ids = list(sim.active_cars.keys())
                        if sim.tracked_car:
                            current_index = car_ids.index(sim.tracked_car)
                            next_index = (current_index + 1) % len(car_ids)
                            sim.tracked_car = car_ids[next_index]
                        else:
                            sim.tracked_car = car_ids[0]
        
        # Process log events
        log_events = log_monitor.get_new_events()
        for event in log_events:
            sim.process_event(event)
            
        # Update simulation
        sim.update(dt)
        
        # Draw everything
        sim.draw(screen)
        
        # Update display
        pygame.display.flip()
        clock.tick(60)

    pygame.quit()
    sys.exit()

if __name__ == "__main__":
    main()
