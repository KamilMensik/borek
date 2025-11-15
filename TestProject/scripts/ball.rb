# frozen_string_literal: true

# Game ball
class Ball < Borek::Scriptable
  def initialize
    @velocity = Borek::Vec2.new(1, 1)
    @speed = 400
    @score = 0
    super
  end

  def check_collision(paddle)
    paddle_transform = paddle.transform
    paddle_position = paddle_transform.position
    paddle_scale = paddle_transform.scale

    diff = Borek::Vec2.new((transform.position.x - paddle_position.x).abs,
                           (transform.position.y - paddle_position.y).abs)

    (diff.x < ((paddle_scale.x + transform.scale.x) / 2)) &&
      (diff.y < ((paddle_scale.y + transform.scale.y) / 2))
  end

  def on_update(delta)
    @velocity.y = -1 if transform.position.y > 380
    @velocity.y = 1 if transform.position.y < -380

    transform.position.x += @velocity.x * @speed * delta
    transform.position.y += @velocity.y * @speed * delta

    paddle_l = Borek::Scene.get_child('Player 1')
    paddle_r = Borek::Scene.get_child('Player 2')
    score_counter = Borek::Scene.get_child('Score')
    
    if check_collision(paddle_l) && @velocity.x.negative?
      @velocity.x = 1
      @speed += 40
      @score += 1
    elsif check_collision(paddle_r) && @velocity.x.positive?
      @velocity.x = -1
      @speed += 40
      @score += 1
    end

    Set.new()

    score_counter.get_component(Borek::TextComponent).text = @score.to_s
  end
end
